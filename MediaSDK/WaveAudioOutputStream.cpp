#include "stdafx.h"
#include "WaveAudioOutputStream.h"

namespace MediaSDK
{
	static const UINT	MaxOpenBufferSize = 1024 * 1024 * 64;
	static const INT	MaxChannelsToMask = 8;
	static const UINT	ChannelsToMask[MaxChannelsToMask + 1] =
	{
		0,
		// 1 = Mono
		SPEAKER_FRONT_CENTER,
		// 2 = Stereo
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		// 3 = Stereo + Center
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER,
		// 4 = Quad
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 5 = 5.0
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 6 = 5.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 7 = 6.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER,
		// 8 = 7.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT
	};

	WaveAudioOutputStream::WaveAudioOutputStream()
		:m_waveO(NULL),
		m_callback(NULL),
		m_dID(0),
		m_count(0),
		m_pending(0),
		m_alignsize(0),
		m_size(0),
		m_state(PCM_NONE)
	{

	}


	WaveAudioOutputStream::~WaveAudioOutputStream()
	{
	}

	void WaveAudioOutputStream::OnOpen()
	{

	}

	void WaveAudioOutputStream::OnClose()
	{

	}

	void WaveAudioOutputStream::HandleError(MMRESULT hRes)
	{
		LOG(ERROR) << "WaveAudioOutputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}
	BOOL WaveAudioOutputStream::Initialize(const AudioParameters& params, UINT count, UINT dID)
	{
		m_dID = dID;
		m_count = count;
		m_params = params;
		const WAVEFORMATEX* pFMT = m_params.GetFormat();
		m_waveFMT.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		m_waveFMT.Format.nChannels = pFMT->nChannels;
		m_waveFMT.Format.nSamplesPerSec = pFMT->nSamplesPerSec;
		m_waveFMT.Format.wBitsPerSample = pFMT->wBitsPerSample;
		m_waveFMT.Format.cbSize = sizeof(m_waveFMT) - sizeof(WAVEFORMATEX);
		m_waveFMT.Format.nBlockAlign = (m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8;//每个采样字节数(多声道)
		m_waveFMT.Format.nAvgBytesPerSec = m_waveFMT.Format.nBlockAlign * m_waveFMT.Format.nSamplesPerSec;
		m_waveFMT.dwChannelMask = ChannelsToMask[m_waveFMT.Format.nChannels > MaxChannelsToMask ? MaxChannelsToMask : m_waveFMT.Format.nChannels];
		m_waveFMT.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		m_waveFMT.Samples.wValidBitsPerSample = m_waveFMT.Format.wBitsPerSample;
		MMRESULT hRes = ::waveOutOpen(NULL, WAVE_MAPPER, (LPWAVEFORMATEX)&m_waveFMT, NULL, 0, WAVE_FORMAT_QUERY);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			return FALSE;
		}
		m_state = PCM_NONE;
		m_size = ((m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8) * m_params.GetFrames();
		m_alignsize = (sizeof(WAVEHDR) + m_size + 15u) & static_cast<size_t>(~15);//16字节对齐
		m_packet.Reset(new AudioPacket(m_size));
		ASSERT(m_packet);
		return TRUE;
	}
	inline WAVEHDR* WaveAudioOutputStream::GetWAVEHDR(INT index) const
	{
		return reinterpret_cast<WAVEHDR*>(&m_bits[index * m_alignsize]);
	}
	void CALLBACK WaveAudioOutputStream::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		WaveAudioOutputStream* stream = reinterpret_cast<WaveAudioOutputStream*>(dwInstance);
		switch (uMsg)
		{
		case MM_WOM_OPEN:
			stream->OnOpen();
			break;
		case MM_WOM_CLOSE:
			stream->OnClose();
			break;
		case MM_WOM_DONE:
			stream->OnDone((LPWAVEHDR)dwParam1, dwInstance);
			break;
		}
	}
	BOOL WaveAudioOutputStream::OnDone(LPWAVEHDR pwh, DWORD_PTR dwInstance)
	{
		TinyAutoLock autolock(m_lock);
		MMRESULT hRes = MMSYSERR_NOERROR;
		if (m_state != PCM_PLAYING)
			return FALSE;
		m_pending -= pwh->dwBufferLength;
		FillPacket(pwh);//读取外部数据
		if (m_state != PCM_PLAYING)
			return FALSE;
		hRes = ::waveOutWrite(m_waveO, pwh, sizeof(WAVEHDR));
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
		}
		m_pending += pwh->dwBufferLength;
		return TRUE;
	}

	void WaveAudioOutputStream::FillPacket(WAVEHDR *pwh)
	{
		ASSERT(pwh);
		const INT64 delay = (m_pending * 1000 * 1000) / m_waveFMT.Format.nAvgBytesPerSec;
		INT32 count = m_callback->OnInput(delay, TinyPerformanceTime::Now(), 0, m_packet);
		UINT32 size = ((m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8) * count;//读到的字节数
		do
		{
			if (size > m_size)//大于分配的字节数
			{
				HandleError(MMSYSERR_NOERROR);
				break;
			}
			memcpy_s(pwh->lpData, size, m_packet->data(), size);
			pwh->dwBufferLength = size;
			pwh->dwFlags = WHDR_PREPARED;
		} while (0);
	}

	BOOL WaveAudioOutputStream::Open()
	{
		if (m_state != PCM_NONE)
			return FALSE;
		if (m_alignsize * m_count > MaxOpenBufferSize)
			return FALSE;
		if (m_count < 2 || m_count > 5)
			return FALSE;
		::GetLastError();
		MMRESULT hRes = ::waveOutOpen(&m_waveO, m_dID, reinterpret_cast<LPCWAVEFORMATEX>(&m_waveFMT), (DWORD_PTR)WaveAudioOutputStream::waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			return FALSE;
		}
		m_bits.Reset(new CHAR[m_alignsize * m_count]);
		ASSERT(m_bits);
		ZeroMemory(m_bits, m_alignsize * m_count);
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			CHAR* ps = reinterpret_cast<CHAR*>(pwh);
			pwh->lpData = ps + sizeof(WAVEHDR);
			pwh->dwBufferLength = m_size;
			pwh->dwBytesRecorded = 0;
			pwh->dwFlags = WHDR_DONE;
			pwh->dwLoops = 0;
			hRes = ::waveOutPrepareHeader(m_waveO, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
				HandleError(hRes);
		}
		m_state = PCM_READY;
		return TRUE;
	}

	BOOL WaveAudioOutputStream::Start(AudioInputCallback* callback)
	{
		if (m_state != PCM_READY)
			return FALSE;
		m_callback = callback;
		m_state = PCM_PLAYING;
		m_pending = 0;
		//读取外部数据
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			ASSERT(pwh);
			FillPacket(pwh);
			m_pending += pwh->dwBufferLength;
		}
		::MemoryBarrier();
		MMRESULT hRes = ::waveOutPause(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			m_state = PCM_READY;
			m_callback = NULL;
			return FALSE;
		}
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			ASSERT(pwh);
			hRes = ::waveOutWrite(m_waveO, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
				return FALSE;
			}
		}
		hRes = ::waveOutRestart(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			return FALSE;
		}
		return TRUE;
	}

	BOOL WaveAudioOutputStream::Stop()
	{
		if (m_state != PCM_PLAYING)
			return FALSE;
		m_state = PCM_STOPPING;
		::MemoryBarrier();
		MMRESULT hRes = ::waveOutReset(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			m_callback = NULL;
			m_state = PCM_READY;
			return FALSE;
		}
		TinyAutoLock autolock(m_lock);
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			ASSERT(pwh);
			pwh->dwFlags = WHDR_PREPARED;
			hRes = ::waveOutUnprepareHeader(m_waveO, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
				m_callback = NULL;
				m_state = PCM_READY;
				return FALSE;
			}
		}
		m_callback = NULL;
		m_state = PCM_READY;
		return TRUE;
	}

	BOOL WaveAudioOutputStream::GetVolume(DOUBLE* volume)
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		if (m_waveO != NULL)
		{
			DWORD dwVolume = 0;
			hRes = waveOutGetVolume(m_waveO, &dwVolume);
			*volume = dwVolume;
			return (hRes == MMSYSERR_NOERROR);
		}
		return FALSE;
	}

	BOOL WaveAudioOutputStream::SetVolume(DOUBLE volume)
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		if (m_waveO != NULL)
		{
			hRes = waveOutSetVolume(m_waveO, static_cast<DWORD>(volume));
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
			}
			return (hRes == MMSYSERR_NOERROR);
		}
		return FALSE;
	}
	void WaveAudioOutputStream::Close()
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		Stop();
		if (m_waveO != NULL)
		{
			for (UINT32 i = 0; i < m_count; ++i)
			{
				WAVEHDR* pwh = GetWAVEHDR(i);
				ASSERT(pwh);
				hRes = ::waveOutUnprepareHeader(m_waveO, pwh, sizeof(WAVEHDR));
				if (hRes != MMSYSERR_NOERROR)
				{
					HandleError(hRes);
				}
			}
			m_bits.Reset(NULL);
			MMRESULT hRes = ::waveOutReset(m_waveO);
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
			}
			hRes = ::waveOutClose(m_waveO);
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
			}
			m_state = PCM_CLOSED;
			m_waveO = NULL;
		}
	}
}