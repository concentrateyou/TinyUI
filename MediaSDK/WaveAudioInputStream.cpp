#include "stdafx.h"
#include "WaveAudioInputStream.h"

namespace MediaSDK
{
	WaveAudioInputStream::WaveAudioInputStream()
		:m_dID(0),
		m_count(0),
		m_size(0),
		m_packetsize(0),
		m_state(PCM_CLOSED),
		m_callback(NULL)
	{
		m_stop.CreateEvent();
	}

	WaveAudioInputStream::~WaveAudioInputStream()
	{
	}

	BOOL WaveAudioInputStream::Initialize(const AudioParameters& params, UINT count, UINT dID)
	{
		const WAVEFORMATEX* pFMT = params.GetFormat();
		m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		m_waveFMT.nChannels = pFMT->nChannels;
		m_waveFMT.nSamplesPerSec = pFMT->nSamplesPerSec;
		m_waveFMT.wBitsPerSample = pFMT->wBitsPerSample;
		m_waveFMT.cbSize = 0;
		m_waveFMT.nBlockAlign = (m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8;
		m_waveFMT.nAvgBytesPerSec = m_waveFMT.nBlockAlign * m_waveFMT.nSamplesPerSec;
		m_dID = dID;
		m_count = count;
		m_params = params;
		m_size = ((m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8) * m_params.GetFrames();
		m_packetsize = sizeof(WAVEHDR) + m_size;
		m_state = PCM_CLOSED;
		return TRUE;
	}

	inline WAVEHDR* WaveAudioInputStream::GetWAVEHDR(INT index) const
	{
		return reinterpret_cast<WAVEHDR*>(&m_bits[index * m_packetsize]);
	}

	void WaveAudioInputStream::HandleError(MMRESULT hRes)
	{
		LOG(ERROR) << "WaveAudioInputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}

	void CALLBACK WaveAudioInputStream::waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		WaveAudioInputStream* stream = reinterpret_cast<WaveAudioInputStream*>(dwInstance);
		switch (uMsg)
		{
		case MM_WIM_OPEN:
			stream->OnOpen();
			break;
		case MM_WIM_CLOSE:
			stream->OnClose();
			break;
		case MM_WIM_DATA:
			stream->OnData((LPWAVEHDR)dwParam1, dwInstance);
			break;
		}
	}
	BOOL WaveAudioInputStream::OnData(LPWAVEHDR pwh, DWORD_PTR dwInstance)
	{
		TinyAutoLock autolock(m_lock);
		MMRESULT hRes = MMSYSERR_NOERROR;
		if (m_state != PCM_RECORDING)
			return FALSE;
		if (m_callback != NULL)
		{
			memcpy_s(m_packet->data(), pwh->dwBufferLength, reinterpret_cast<CHAR*>(pwh->lpData), pwh->dwBufferLength);
			m_callback->OnOutput(TinyPerformanceTime::Now(), m_packet);
		}
		else
		{
			m_stop.SetEvent();
		}
		return TRUE;
	}

	BOOL WaveAudioInputStream::Open()
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		hRes = ::waveInOpen(&m_waveI, m_dID, &m_waveFMT, reinterpret_cast<DWORD_PTR>(&WaveAudioInputStream::waveInProc), reinterpret_cast<DWORD_PTR>(this),
			CALLBACK_FUNCTION);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		m_bits.Reset(new CHAR[m_size * m_count]);
		ASSERT(m_bits);
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			CHAR* ps = reinterpret_cast<CHAR*>(pwh);
			pwh->lpData = ps + sizeof(WAVEHDR);
			pwh->dwBufferLength = m_size;
			pwh->dwBytesRecorded = 0;
			pwh->dwFlags = WHDR_DONE;
			pwh->dwLoops = 0;
			hRes = ::waveInPrepareHeader(m_waveI, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
				return FALSE;
			}
		}
		return TRUE;
	}


	BOOL WaveAudioInputStream::Start(AudioOutputCallback* callback)
	{
		if (m_state != PCM_READY)
			return FALSE;
		m_callback = callback;
		m_state = PCM_RECORDING;
		MMRESULT hRes = MMSYSERR_NOERROR;
		for (UINT32 i = 0; i < m_count; ++i)
		{
			WAVEHDR* pwh = GetWAVEHDR(i);
			hRes = ::waveInAddBuffer(m_waveI, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
				m_callback = NULL;
				m_state = PCM_READY;
				return FALSE;
			}
		}
		hRes = ::waveInStart(m_waveI);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			m_state = PCM_READY;
			m_callback = NULL;
		}
		return TRUE;
	}

	BOOL WaveAudioInputStream::Stop()
	{
		if (m_state != PCM_RECORDING)
			return FALSE;
		{
			TinyAutoLock autolock(m_lock);
			m_callback = NULL;
		}
		m_stop.WaitEvent(INFINITE);
		MMRESULT hRes = ::waveInReset(m_waveI);
		if (hRes != MMSYSERR_NOERROR)
		{
			HandleError(hRes);
			m_callback = NULL;
			m_state = PCM_READY;
			return FALSE;
		}

		m_state = PCM_READY;
		return TRUE;
	}

	void WaveAudioInputStream::Close()
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		Stop();
		if (m_waveI != NULL)
		{
			for (UINT32 i = 0; i < m_count; ++i)
			{
				WAVEHDR* pwh = GetWAVEHDR(i);
				hRes = ::waveInUnprepareHeader(m_waveI, pwh, sizeof(WAVEHDR));
				if (hRes != MMSYSERR_NOERROR)
				{
					HandleError(hRes);
				}
			}
			hRes = ::waveInReset(m_waveI);
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
			}
			hRes = ::waveInClose(m_waveI);
			if (hRes != MMSYSERR_NOERROR)
			{
				HandleError(hRes);
			}
			m_state = PCM_CLOSED;
			m_waveI = NULL;
		}
	}

	void WaveAudioInputStream::OnOpen()
	{

	}

	void WaveAudioInputStream::OnClose()
	{

	}

	BOOL WaveAudioInputStream::SetVolume(FLOAT volume)
	{
		return TRUE;
	}

	BOOL WaveAudioInputStream::GetVolume(FLOAT* volume)
	{
		return TRUE;
	}

}