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
		:m_handle(NULL),
		m_waveO(NULL),
		m_callback(NULL),
		m_dID(0),
		m_count(0),
		m_pending(0),
		m_size(0),
		m_state(PCM_NONE)
	{
		m_event.CreateEvent();
	}


	WaveAudioOutputStream::~WaveAudioOutputStream()
	{
	}

	void WaveAudioOutputStream::OutputError(MMRESULT hRes)
	{
		LOG(ERROR) << "WaveAudioOutputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}
	void WaveAudioOutputStream::QueueNextPacket(WAVEHDR *buffer)
	{
		const INT64 delay = (m_pending * 1000 * 1000) / m_waveFMT.Format.nAvgBytesPerSec;
		INT32 count = m_callback->OnInput(delay, TinyPerformanceTime::Now(), 0, m_packet);
		UINT32 size = count * m_waveFMT.Format.nBlockAlign;//输入字节数
		UINT32 buffersize = m_waveFMT.Format.nBlockAlign * m_params.GetFrames();
		if (size <= buffersize)
		{
			buffer->dwBufferLength = size;
		}
		else
		{
			OutputError(0);
		}
		buffer->dwFlags = WHDR_PREPARED;
	}
	BOOL WaveAudioOutputStream::Initialize(const AudioParameters& params, UINT count, UINT dID)
	{
		m_dID = dID;
		m_count = count;
		m_params = params;
		const WAVEFORMATEX* waveFMT = m_params.GetFormat();
		m_waveFMT.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		m_waveFMT.Format.nChannels = waveFMT->nChannels;
		m_waveFMT.Format.nSamplesPerSec = waveFMT->nSamplesPerSec;
		m_waveFMT.Format.wBitsPerSample = waveFMT->wBitsPerSample;
		m_waveFMT.Format.cbSize = sizeof(waveFMT) - sizeof(WAVEFORMATEX);
		m_waveFMT.Format.nBlockAlign = (m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8;
		m_waveFMT.Format.nAvgBytesPerSec = m_waveFMT.Format.nBlockAlign * m_waveFMT.Format.nSamplesPerSec;
		m_waveFMT.dwChannelMask = ChannelsToMask[m_waveFMT.Format.nChannels > MaxChannelsToMask ? MaxChannelsToMask : m_waveFMT.Format.nChannels];
		m_waveFMT.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		m_waveFMT.Samples.wValidBitsPerSample = m_waveFMT.Format.wBitsPerSample;
		//内存对齐
		m_size = (sizeof(WAVEHDR) + m_waveFMT.Format.nBlockAlign * m_params.GetFrames() + 15u) & static_cast<size_t>(~15);
		return TRUE;
	}
	inline WAVEHDR* WaveAudioOutputStream::GetWAVEHDR(INT index) const
	{
		return reinterpret_cast<WAVEHDR*>(&m_buffer[index * m_size]);
	}
	void NTAPI WaveAudioOutputStream::OnCallback(PVOID lpParameter, BOOLEAN timerFired)
	{
		WaveAudioOutputStream* stream = reinterpret_cast<WaveAudioOutputStream*>(lpParameter);
		TinyAutoLock lock(stream->m_lock);
		if (stream->m_state == PCM_PLAYING)
		{
			for (UINT i = 0; i <= stream->m_count; ++i)
			{
				WAVEHDR* buffer = stream->GetWAVEHDR(i);
				if (buffer->dwFlags & WHDR_DONE)
				{
					stream->m_pending -= buffer->dwBufferLength;
					stream->QueueNextPacket(buffer);
					if (stream->m_state == PCM_PLAYING)
					{
						MMRESULT hRes = ::waveOutWrite(stream->m_waveO, buffer, sizeof(WAVEHDR));
						if (hRes != MMSYSERR_NOERROR)
							stream->OutputError(hRes);
						stream->m_pending += buffer->dwBufferLength;
					}
				}
			}
		}
	}

	BOOL WaveAudioOutputStream::Open()
	{
		if (m_state != PCM_NONE)
			return FALSE;
		if (m_size * m_count > MaxOpenBufferSize)
			return FALSE;
		if (m_count < 2 || m_count > 5)
			return FALSE;
		::GetLastError();
		MMRESULT hRes = ::waveOutOpen(&m_waveO, m_dID, reinterpret_cast<LPCWAVEFORMATEX>(&m_waveFMT), reinterpret_cast<DWORD_PTR>(m_event.Handle()), NULL, CALLBACK_EVENT);
		if (hRes != MMSYSERR_NOERROR)
		{
			OutputError(hRes);
			return FALSE;
		}
		m_buffer.Reset(m_size * m_count);
		ASSERT(m_buffer);
		for (UINT i = 0; i <= m_count; ++i)
		{
			WAVEHDR* buffer = GetWAVEHDR(i);
			buffer->lpData = reinterpret_cast<char*>(buffer) + sizeof(WAVEHDR);
			buffer->dwBufferLength = m_waveFMT.Format.nBlockAlign * m_params.GetFrames();
			buffer->dwBytesRecorded = 0;
			buffer->dwFlags = WHDR_DONE;
			buffer->dwLoops = 0;
			hRes = ::waveOutPrepareHeader(m_waveO, buffer, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				OutputError(hRes);
			}
		}
		m_state = PCM_READY;
		return TRUE;
	}

	BOOL WaveAudioOutputStream::Start(AudioInputCallback* callback)
	{
		m_callback = callback;
		if (!m_event.ResetEvent())
		{
			OutputError(MMSYSERR_ERROR);
			return FALSE;
		}
		if (!::RegisterWaitForSingleObject(&m_handle, m_event, &OnCallback, this, INFINITE, WT_EXECUTEDEFAULT))
		{
			OutputError(MMSYSERR_ERROR);
			return FALSE;
		}
		m_state = PCM_PLAYING;
		m_pending = 0;
		for (UINT i = 0; i <= m_count; ++i)
		{
			WAVEHDR* buffer = GetWAVEHDR(i);
			QueueNextPacket(buffer);
			m_pending += buffer->dwBufferLength;
		}
		::MemoryBarrier();
		MMRESULT hRes = ::waveOutPause(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			OutputError(hRes);
			return FALSE;
		}
		for (UINT i = 0; i <= m_count; ++i)
		{
			WAVEHDR* buffer = GetWAVEHDR(i);
			hRes = ::waveOutWrite(m_waveO, buffer, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				OutputError(hRes);
			}
		}
		hRes = ::waveOutRestart(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			OutputError(hRes);
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
		if (m_handle != NULL)
		{
			if (!::UnregisterWaitEx(m_handle, INVALID_HANDLE_VALUE))
			{
				OutputError(::GetLastError());
			}
			m_handle = NULL;
		}
		MMRESULT hRes = ::waveOutReset(m_waveO);
		if (hRes != MMSYSERR_NOERROR)
		{
			OutputError(hRes);
		}
		TinyAutoLock lock(m_lock);
		for (UINT i = 0; i <= m_count; ++i)
		{
			WAVEHDR* buffer = GetWAVEHDR(i);
			buffer->dwFlags = WHDR_PREPARED;
			MMRESULT hRes = ::waveOutUnprepareHeader(m_waveO, buffer, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				OutputError(hRes);
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
			return hRes == MMSYSERR_NOERROR;
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
				OutputError(hRes);
			}
			return hRes == MMSYSERR_NOERROR;
		}
		return FALSE;
	}

	void WaveAudioOutputStream::Close()
	{
		Stop();
		if (m_waveO != NULL)
		{
			for (UINT i = 0; i <= m_count; ++i)
			{
				WAVEHDR* s = GetWAVEHDR(i);
				::waveOutUnprepareHeader(m_waveO, s, sizeof(WAVEHDR));
			}
			m_buffer.Release();
			MMRESULT hRes = ::waveOutReset(m_waveO);
			if (hRes != MMSYSERR_NOERROR)
				OutputError(hRes);
			hRes = ::waveOutClose(m_waveO);
			if (hRes != MMSYSERR_NOERROR)
				OutputError(hRes);
			m_state = PCM_CLOSED;
			m_waveO = NULL;
		}
	}
}