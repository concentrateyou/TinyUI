#include "stdafx.h"
#include "WASAPIAudioOutputStream.h"

namespace MediaSDK
{
	WASAPIAudioOutputStream::WASAPIAudioOutputStream()
		:m_state(AUDIO_CLOSED),
		m_callback(NULL),
		m_count(0),
		m_writes(0),
		m_latency(0)
	{
		m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
		m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
	}

	WASAPIAudioOutputStream::~WASAPIAudioOutputStream()
	{

	}

	BOOL WASAPIAudioOutputStream::GetAudioClient()
	{
		HRESULT hRes = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		REFERENCE_TIME hns = m_engineLatency * MFTIMES_PER_MS;
		switch (m_mode)
		{
		case AUDCLNT_SHAREMODE_SHARED:
		{
			hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hns, 0, m_params.GetFormat(), &m_sessionID);
			if (hRes == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
			{
				hRes = m_audioClient->GetBufferSize(&m_count);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				hns = (REFERENCE_TIME)(static_cast<FLOAT>(MFTIMES_PER_SEC) *  m_count / m_params.GetFormat()->nSamplesPerSec + 0.5);
				m_audioClient.Release();
				hRes = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hns, 0, m_params.GetFormat(), &m_sessionID);
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
			hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hns, hns, m_params.GetFormat(), &m_sessionID);
			if (hRes == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
			{
				hRes = m_audioClient->GetBufferSize(&m_count);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				hns = (REFERENCE_TIME)(static_cast<FLOAT>(MFTIMES_PER_SEC) *  m_count / m_params.GetFormat()->nSamplesPerSec + 0.5);
				m_audioClient.Release();
				hRes = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				hRes = m_audioClient->Initialize(m_mode, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hns, hns, m_params.GetFormat(), &m_sessionID);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
			}
		}
		break;
		}
		return TRUE;
	}

	BOOL WASAPIAudioOutputStream::Initialize(const AudioParameters& params, const string& deviceID, AUDCLNT_SHAREMODE mode, UINT32 engineLatency)
	{
		m_params = params;
		m_deviceID = deviceID;
		m_state = AUDIO_CLOSED;
		m_mode = mode;
		m_engineLatency = engineLatency;
		return TRUE;
	}
	BOOL WASAPIAudioOutputStream::Open()
	{
		if (m_state != AUDIO_CLOSED)
			return FALSE;
		HRESULT hRes = S_OK;
		wstring wID = UTF8ToUTF16(m_deviceID);
		DWORD state = DEVICE_STATE_DISABLED;
		TinyComPtr<IMMDeviceEnumerator> enumerator;
		hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
		if (hRes == CO_E_NOTINITIALIZED && !enumerator)
		{
			hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			if (SUCCEEDED(hRes))
			{
				hRes = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
			}
		}
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
		if (!GetAudioClient())
		{
			goto _ERROR;
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
		hRes = m_audioClient->GetStreamLatency(&m_latency);
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
		m_writes = 0;
		m_state = AUDIO_READY;
		m_packet.Reset(new AudioPacket(m_params));
		ASSERT(m_packet);
		return TRUE;
	_ERROR:
		m_callback = NULL;
		m_audioRenderClient.Release();
		m_audioClient.Release();
		m_audioClock.Release();
		m_audioVolume.Release();
		m_audioDevice.Release();
		m_state = AUDIO_CLOSED;
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

	BOOL WASAPIAudioOutputStream::FillSilent(IAudioClient* client, IAudioRenderClient* renderClient)
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
		m_writes += size;
		return TRUE;
	}

	BOOL WASAPIAudioOutputStream::Start(AudioInputCallback* callback)
	{
		if (m_state != AUDIO_READY)
			return FALSE;
		HRESULT hRes = S_OK;
		m_writes = 0;
		m_callback = callback;
		m_state = AUDIO_PLAYING;
		if (!m_sampleReady.ResetEvent())
		{
			HandleError(::GetLastError());
			goto _ERROR;
		}
		if (m_mode == AUDCLNT_SHAREMODE_SHARED)
		{
			if (!FillSilent(m_audioClient, m_audioRenderClient))
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
		m_sampleReady.ResetEvent();
		m_audioStop.ResetEvent();
		m_runnable.Close(INFINITE);
		m_callback = NULL;
		m_state = AUDIO_READY;
		return FALSE;
	}

	BOOL WASAPIAudioOutputStream::Stop()
	{
		if (m_state != AUDIO_PLAYING)
			return FALSE;
		m_state = AUDIO_STOPPING;
		::MemoryBarrier();
		HRESULT hRes = S_OK;
		m_audioStop.SetEvent();
		m_runnable.Close(INFINITE);
		hRes = m_audioClient->Stop();
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->Reset();
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (m_mode == AUDCLNT_SHAREMODE_SHARED)
		{
			UINT32 padding = 0;
			m_audioClient->GetCurrentPadding(&padding);
		}
	_ERROR:
		m_state = AUDIO_READY;
		m_callback = NULL;
		return SUCCEEDED(hRes);
	}

	BOOL WASAPIAudioOutputStream::GetVolume(FLOAT* volume)
	{
		if (!m_audioVolume)
			return FALSE;
		return SUCCEEDED(m_audioVolume->GetMasterVolume(volume));
	}

	BOOL WASAPIAudioOutputStream::SetVolume(FLOAT volume)
	{
		if (!m_audioVolume)
			return FALSE;
		return SUCCEEDED(m_audioVolume->SetMasterVolume(volume, NULL));
	}

	void WASAPIAudioOutputStream::Close()
	{
		Stop();
		m_writes = 0;
		m_callback = NULL;
		m_sampleReady.ResetEvent();
		m_audioStop.ResetEvent();
		m_audioRenderClient.Release();
		m_audioClient.Release();
		m_audioClock.Release();
		m_audioVolume.Release();
		m_audioDevice.Release();
		m_state = AUDIO_CLOSED;
	}

	BOOL WASAPIAudioOutputStream::FillPackage(const WAVEFORMATEX* waveFMT, REFERENCE_TIME latency, UINT64 lFrequency)
	{
		HRESULT hRes = S_OK;
		BYTE*	data = NULL;
		UINT32	padding = 0;
		UINT32	available = 0;
		UINT64  pos64 = 0;
		UINT64  qpc64 = 0;
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
				UINT64 delays = 0;
				hRes = m_audioClock->GetPosition(&pos64, &qpc64);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					return FALSE;
				}
				const UINT64 plays = waveFMT->nSamplesPerSec * pos64 / lFrequency;
				delays = m_writes - plays;
				UINT32 size = available * waveFMT->nBlockAlign;
				m_writes += available;
				m_packet->SetSize(size);
				INT32 count = m_callback->OnInput(delays, TinyTime::Now(), m_packet);
				size = min(count * waveFMT->nBlockAlign, size);
				memcpy_s(data, size, m_packet->data(), size);
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
			hRes = m_audioRenderClient->GetBuffer(available, &data);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			UINT64 delays = 0;
			hRes = m_audioClock->GetPosition(&pos64, &qpc64);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			const UINT64 plays = waveFMT->nSamplesPerSec * pos64 / lFrequency;//ÒÑ²¥·Å
			delays = m_count - plays;
			UINT32 size = available * waveFMT->nBlockAlign;
			m_packet->SetSize(size);
			INT32 count = m_callback->OnInput(delays, TinyTime::Now(), m_packet);
			size = min(count * waveFMT->nBlockAlign, size);
			memcpy_s(data, size, m_packet->data(), size);
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
				bPlaying = FillPackage(m_params.GetFormat(), m_latency, m_lFrequency);
				break;
			default:
				bPlaying = FALSE;
				break;
			}
		}
	}
}