#include "stdafx.h"
#include "WASAPIAudioOutputStream.h"

namespace MediaSDK
{
	WASAPIAudioOutputStream::WASAPIAudioOutputStream()
		:m_state(AUDIO_NONE),
		m_callback(NULL),
		m_count(0),
		m_wFrames(0)
	{
		m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
		m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
	}

	WASAPIAudioOutputStream::~WASAPIAudioOutputStream()
	{

	}

	BOOL WASAPIAudioOutputStream::Initialize(WORD wFrames, const string& deviceID, AUDCLNT_SHAREMODE mode)
	{
		m_wFrames = wFrames;
		m_deviceID = deviceID;
		m_state = AUDIO_NONE;
		m_mode = mode;
		return TRUE;
	}
	BOOL WASAPIAudioOutputStream::BuildFormat()
	{
		if (!m_audioClient)
			return FALSE;
		WAVEFORMATEX* pFMT = NULL;
		HRESULT hRes = m_audioClient->GetMixFormat(&pFMT);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		UINT32 size = sizeof(WAVEFORMATEX) + pFMT->cbSize;
		memcpy(&m_waveFMT, pFMT, size);
		return TRUE;
	}
	const WAVEFORMATPCMEX* WASAPIAudioOutputStream::GetFormat() const
	{
		return &m_waveFMT;
	}
	BOOL WASAPIAudioOutputStream::Open()
	{
		if (m_state != AUDIO_NONE)
			return FALSE;
		WAVEFORMATEX* pFMT = NULL;
		wstring wID = UTF8ToUTF16(m_deviceID);
		DWORD state = DEVICE_STATE_DISABLED;
		REFERENCE_TIME request = ((REFERENCE_TIME)(static_cast<double>(MFTIMES_PER_SEC) * m_wFrames / m_waveFMT.Format.nSamplesPerSec + 0.5));
		TinyComPtr<IMMDeviceEnumerator> enumerator;
		HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = enumerator->GetDevice(wID.c_str(), &m_audioDevice);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioDevice->GetState(&state);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (!(state & DEVICE_STATE_ACTIVE))
		{
			goto _ERROR;
		}
		hRes = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (!BuildFormat())
		{
			goto _ERROR;
		}
		switch (m_mode)
		{
		case AUDCLNT_SHAREMODE_SHARED:
		{
			hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, pFMT, &m_sessionID);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
		}
		break;
		case AUDCLNT_SHAREMODE_EXCLUSIVE:
		{
			hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, request, request, pFMT, &m_sessionID);
			if (FAILED(hRes))
			{
				if (hRes != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
				{
					HandleError(hRes);
					goto _ERROR;
				}
			}
		}
		break;
		}
		hRes = m_audioClient->GetBufferSize(&m_count);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->SetEventHandle(m_sampleReady);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetService(IID_PPV_ARGS(&m_audioClock));
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_audioRenderClient);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_audioVolume);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClock->GetFrequency(&m_lFrequency);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		m_state = AUDIO_READY;
		return TRUE;
	_ERROR:
		m_state = AUDIO_NONE;
		return FALSE;
	}

	void WASAPIAudioOutputStream::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "WASAPIAudioOutputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}

	BOOL WASAPIAudioOutputStream::FillSilence(IAudioClient* client, IAudioRenderClient* renderClient)
	{
		UINT32 bufferSize = 0;
		HRESULT hRes = client->GetBufferSize(&bufferSize);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		UINT32 padding = 0;
		hRes = client->GetCurrentPadding(&padding);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		BYTE* data = NULL;
		INT32 size = bufferSize - padding;
		hRes = renderClient->GetBuffer(size, &data);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		hRes = renderClient->ReleaseBuffer(size, AUDCLNT_BUFFERFLAGS_SILENT);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		return TRUE;
	}

	BOOL WASAPIAudioOutputStream::Start(AudioInputCallback* callback)
	{
		if (m_state != AUDIO_READY)
			return FALSE;
		m_callback = callback;
		m_state = AUDIO_PLAYING;
		HRESULT hRes = S_OK;
		if (m_mode == AUDCLNT_SHAREMODE_SHARED)
		{
			if (!FillSilence(m_audioClient, m_audioRenderClient))
				goto _ERROR;
		}
		if (!m_runnable.Submit(BindCallback(&WASAPIAudioOutputStream::OnMessagePump, this)))
			goto _ERROR;
		hRes = m_audioClient->Start();
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		return TRUE;
	_ERROR:
		m_state = AUDIO_READY;
		return FALSE;
	}

	BOOL WASAPIAudioOutputStream::Stop()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioOutputStream::GetVolume(DOUBLE* volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioOutputStream::SetVolume(DOUBLE volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void WASAPIAudioOutputStream::Close()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioOutputStream::Render(const WAVEFORMATPCMEX& waveFMT, UINT64 lFrequency)
	{
		HRESULT hRes = S_OK;
		BYTE* data = NULL;
		UINT32 padding = 0;
		UINT32 available = 0;
		switch (m_mode)
		{
		case AUDCLNT_SHAREMODE_SHARED:
		{
			hRes = m_audioClient->GetCurrentPadding(&padding);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			available = m_count - padding;
			if (available > 0)
			{
				hRes = m_audioRenderClient->GetBuffer(available, &data);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				UINT32 size = available * m_waveFMT.Format.nBlockAlign;
				
				hRes = m_audioRenderClient->ReleaseBuffer(available, 0);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
			}
		}
		break;
		case AUDCLNT_SHAREMODE_EXCLUSIVE:
		{
			UINT32 available = m_count;
			hRes = m_audioRenderClient->GetBuffer(available, &data);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			INT64  delayTimestamp = 0;
			UINT64 position = 0;
			UINT64 qpc = 0;
			hRes = m_audioClock->GetPosition(&position, &qpc);
			if (FAILED(hRes))
			{
				delayTimestamp = TinyPerformanceTime::Now();
			}
			else
			{
				const UINT64 plays = waveFMT.Format.nSamplesPerSec * position / lFrequency;//ÒÑ²¥·Å
				const UINT64 delays = m_count - plays;
				delayTimestamp = delays * TinyTime::MicrosecondsPerSecond / waveFMT.Format.nSamplesPerSec;
			}

			hRes = m_audioRenderClient->ReleaseBuffer(m_count, 0);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
		}
		break;
		}
		return TRUE;
	}

	void WASAPIAudioOutputStream::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		BOOL bPlaying = TRUE;
		HANDLE waits[2] = { m_audioStop ,m_sampleReady };
		TinyScopedAvrt avrt("Pro Audio");
		avrt.SetPriority();
		while (bPlaying)
		{
			DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
			switch (dwRes)
			{
			case WAIT_OBJECT_0 + 0:
			case WAIT_FAILED:
			case WAIT_ABANDONED:
				bPlaying = FALSE;
				break;
			case WAIT_TIMEOUT:
			case WAIT_OBJECT_0 + 1:
				bPlaying = Render(m_waveFMT, m_lFrequency);
				break;
			}
		}
	}
}