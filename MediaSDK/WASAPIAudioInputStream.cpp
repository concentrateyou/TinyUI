#include "stdafx.h"
#include "WASAPIAudioInputStream.h"

namespace MediaSDK
{
	WASAPIAudioInputStream::WASAPIAudioInputStream()
		:m_state(AUDIO_CLOSED),
		m_callback(NULL),
		m_count(0),
		m_lFrequency(0),
		m_dwStreamFlag(AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST)
	{
		m_sampleReady.CreateEvent();
		m_audioStop.CreateEvent();
	}
	WASAPIAudioInputStream::~WASAPIAudioInputStream()
	{

	}

	BOOL WASAPIAudioInputStream::Initialize(const AudioParameters& params, const string& deviceID)
	{
		m_params = params;
		m_deviceID = deviceID;
		m_state = AUDIO_CLOSED;

		return TRUE;
	}

	BOOL WASAPIAudioInputStream::Open()
	{
		if (m_state != AUDIO_CLOSED)
			return FALSE;
		HRESULT hRes = S_OK;
		DWORD state = DEVICE_STATE_DISABLED;
		TinyComPtr<IMMDeviceEnumerator> enumerator;
		hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
		{
			if (hRes == CO_E_NOTINITIALIZED && !enumerator)
			{
				hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
				if (SUCCEEDED(hRes))
				{
					hRes = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
				}
			}
		}
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (Loopback(enumerator, m_deviceID))
		{
			m_dwStreamFlag |= AUDCLNT_STREAMFLAGS_LOOPBACK;
		}
		else
		{
			m_dwStreamFlag &= ~AUDCLNT_STREAMFLAGS_LOOPBACK;
		}
		hRes = enumerator->GetDevice(UTF8ToUTF16(m_deviceID).c_str(), &m_audioDevice);
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
		hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwStreamFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, m_params.GetFormat(), NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
		{
			hRes = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClientLB);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
			hRes = m_audioClientLB->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, m_params.GetFormat(), NULL);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
		}
		hRes = m_audioClientLB->SetEventHandle(m_sampleReady);
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
		hRes = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_audioCapture);
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
		hRes = m_audioClient->GetService(__uuidof(IChannelAudioVolume), (void**)&m_channelVolume);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetService(__uuidof(IAudioSessionControl), (void**)&m_audioSession);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetService(__uuidof(IAudioClock), (void**)&m_audioClock);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_audioClient->GetBufferSize(&m_count);
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
		m_packet.Reset(new AudioPacket(m_params));
		ASSERT(m_packet);
		return TRUE;
	_ERROR:
		m_callback = NULL;
		m_audioClientLB.Release();
		m_audioClient.Release();
		m_audioClock.Release();
		m_audioVolume.Release();
		m_audioCapture.Release();
		m_audioSession.Release();
		m_channelVolume.Release();
		m_audioDevice.Release();
		m_state = AUDIO_CLOSED;
		return FALSE;
	}

	BOOL WASAPIAudioInputStream::Start(AudioOutputCallback* callback)
	{
		if (m_state != AUDIO_READY)
			return FALSE;
		m_callback = callback;
		m_state = AUDIO_RECORDING;
		HRESULT hRes = S_OK;
		if (!m_sampleReady.ResetEvent())
		{
			HandleError(::GetLastError());
			goto _ERROR;
		}
		if (!m_runnable.Submit(BindCallback(&WASAPIAudioInputStream::OnMessagePump, this)))
		{
			goto _ERROR;
		}
		hRes = m_audioClient->Start();
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_LOOPBACK && m_audioClientLB)
		{
			hRes = m_audioClientLB->Start();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
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

	BOOL WASAPIAudioInputStream::Stop()
	{
		if (m_state != AUDIO_RECORDING)
			return FALSE;
		m_state = AUDIO_STOPPING;
		::MemoryBarrier();
		m_audioStop.SetEvent();
		m_runnable.Close(INFINITE);
		HRESULT hRes = S_OK;
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
		if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_LOOPBACK && m_audioClientLB != NULL)
		{
			hRes = m_audioClientLB->Stop();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
			hRes = m_audioClientLB->Reset();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
		}
	_ERROR:
		m_callback = NULL;
		m_state = AUDIO_READY;
		return SUCCEEDED(hRes);
	}

	void WASAPIAudioInputStream::Close()
	{
		Stop();
		m_audioClient.Release();
		m_audioClientLB.Release();
		m_audioClock.Release();
		m_audioCapture.Release();
		m_audioSession.Release();
		m_audioVolume.Release();
		m_channelVolume.Release();
		m_sampleReady.ResetEvent();
		m_audioStop.ResetEvent();
		m_callback = NULL;
		m_state = AUDIO_CLOSED;
	}

	BOOL WASAPIAudioInputStream::SetVolume(FLOAT volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioInputStream::GetVolume(FLOAT* volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
	BOOL WASAPIAudioInputStream::FillPackage(const WAVEFORMATEX* waveFMT, UINT64 lFrequency)
	{
		HRESULT hRes = S_OK;
		DWORD	dwFlags = 0;
		UINT32	available = 0;
		BYTE*	bits = NULL;
		UINT64  pu64Pos = 0;
		UINT64	pu64QPCPos = 0;
		for (;;)
		{
			hRes = m_audioCapture->GetBuffer(&bits, &available, &dwFlags, &pu64Pos, &pu64QPCPos);
			if (hRes == AUDCLNT_S_BUFFER_EMPTY)
				break;
			hRes = m_audioClock->GetPosition(&pu64Pos, &pu64QPCPos);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			UINT32 size = available * waveFMT->nBlockAlign;
			if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				m_packet->SetSize(size);
				memset(m_packet->data(), 0, m_packet->size());
			}
			else
			{
				m_packet->SetSize(size);
				memcpy_s(m_packet->data(), m_packet->size(), bits, m_packet->size());
			}
			if (m_callback != NULL)
			{
				m_callback->OnOutput(TinyPerformanceTime::Now(), m_packet);
			}
			m_audioCapture->ReleaseBuffer(available);
		}
		return TRUE;
	}
	void WASAPIAudioInputStream::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		BOOL bRecording = TRUE;
		HANDLE waits[2] = { m_audioStop ,m_sampleReady };
		TinyScopedAvrt avrt("Pro Audio");
		avrt.SetPriority();
		while (bRecording)
		{
			DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
			switch (dwRes)
			{
			case WAIT_OBJECT_0 + 0:
			case WAIT_FAILED:
			case WAIT_ABANDONED:
				bRecording = FALSE;
				break;
			case WAIT_TIMEOUT:
			case WAIT_OBJECT_0 + 1:
				bRecording = FillPackage(m_params.GetFormat(), m_lFrequency);
				break;
			default:
				bRecording = FALSE;
				break;
			}
		}
	}
	void WASAPIAudioInputStream::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "WASAPIAudioInputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}
	BOOL WASAPIAudioInputStream::Loopback(IMMDeviceEnumerator* enumerator, const string& deviceID)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<IMMDevice> device;
		hRes = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		WCHAR* wID = NULL;
		if (FAILED(device->GetId(&wID)))
		{
			HandleError(hRes);
			return FALSE;
		}
		string dID = UTF16ToUTF8(wID);
		CoTaskMemFree(wID);
		return dID == deviceID;
	}
}