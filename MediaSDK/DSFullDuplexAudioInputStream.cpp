#include "stdafx.h"
#include "DSFullDuplexAudioInputStream.h"

namespace MediaSDK
{
	DSFullDuplexAudioInputStream::DSFullDuplexAudioInputStream()
		:m_count(0),
		m_size(0),
		m_dwOffset(0),
		m_state(PCM_NONE),
		m_callback(NULL),
		m_hWND(NULL),
		m_captureID(DSDEVID_DefaultCapture),
		m_renderID(DSDEVID_DefaultPlayback)
	{
		m_event.CreateEvent();
	}

	DSFullDuplexAudioInputStream::~DSFullDuplexAudioInputStream()
	{
	}

	BOOL DSFullDuplexAudioInputStream::Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& captureID, const GUID& renderID)
	{
		const WAVEFORMATEX* pFMT = params.GetFormat();
		ASSERT(pFMT);
		m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		m_waveFMT.nChannels = pFMT->nChannels;
		m_waveFMT.nSamplesPerSec = pFMT->nSamplesPerSec;
		m_waveFMT.wBitsPerSample = pFMT->wBitsPerSample;
		m_waveFMT.cbSize = 0;
		m_waveFMT.nBlockAlign = (m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8;
		m_waveFMT.nAvgBytesPerSec = m_waveFMT.nBlockAlign * m_waveFMT.nSamplesPerSec;
		m_captureID = captureID;
		m_renderID = renderID;
		m_hWND = hWND;
		m_count = count;
		m_params = params;
		m_state = PCM_NONE;
		m_size = ((m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8) * m_params.GetFrames();
		m_packet.Reset(new AudioPacket(params));
		ASSERT(m_packet);
		return TRUE;
	}

	void DSFullDuplexAudioInputStream::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "DSFullDuplexAudioInputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}

	void DSFullDuplexAudioInputStream::OnCallback(BOOLEAN timerFired)
	{
		HRESULT hRes = S_OK;
		do
		{
			if (m_state != PCM_RECORDING)
				break;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			DWORD	dwReadPos = 0;
			DWORD	dwStatus = 0;
			hRes = m_dscb8->GetStatus(&dwStatus);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				break;
			}
			if (!(dwStatus & DSCBSTATUS_CAPTURING))
			{
				break;
			}
			hRes = m_dscb8->GetCurrentPosition(NULL, &dwReadPos);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				break;
			}
			LONG locksize = static_cast<LONG>(dwReadPos - m_dwOffset);
			if (locksize < 0)
			{
				locksize += static_cast<LONG>(m_size * m_count);
			}
			if (locksize == 0)
			{
				break;
			}
			hRes = m_dscb8->Lock(m_dwOffset, locksize, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				break;
			}
			m_queue.Push(static_cast<CHAR*>(ppvAudioPtr), dwAudioBytes);
			hRes = m_dscb8->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			if (FAILED(hRes))
			{
				m_queue.Pop(dwAudioBytes);
				HandleError(hRes);
				break;
			}
			const CHAR* pval = NULL;
			INT32 psize = 0;
			m_queue.Peek(&pval, &psize);
			if (psize >= m_size)
			{
				memcpy_s(m_packet->data(), m_size, pval, m_size);
				m_queue.Pop(m_size);
				if (m_callback != NULL)
				{
					m_callback->OnOutput(TinyPerformanceTime::Now(), m_packet);
				}
			}
			m_dwOffset += dwAudioBytes;
			m_dwOffset %= (m_size * m_count);
		} while (0);
	}

	BOOL DSFullDuplexAudioInputStream::Open()
	{
		DSCCAPS caps;
		ZeroMemory(&caps, sizeof(DSCCAPS));
		caps.dwSize = sizeof(DSCCAPS);
		DSCEFFECTDESC dsceds[2];
		ZeroMemory(&dsceds[0], sizeof(DSCEFFECTDESC));
		dsceds[0].dwSize = sizeof(DSCEFFECTDESC);
		dsceds[0].dwFlags = DSCFX_LOCSOFTWARE;
		dsceds[0].guidDSCFXClass = GUID_DSCFX_CLASS_AEC;
		dsceds[0].guidDSCFXInstance = GUID_DSCFX_MS_AEC;
		ZeroMemory(&dsceds[1], sizeof(DSCEFFECTDESC));
		dsceds[1].dwSize = sizeof(DSCEFFECTDESC);
		dsceds[1].dwFlags = DSCFX_LOCSOFTWARE;
		dsceds[1].guidDSCFXClass = GUID_DSCFX_CLASS_NS;
		dsceds[1].guidDSCFXInstance = GUID_DSCFX_MS_NS;

		DSCBUFFERDESC dscb;
		ZeroMemory(&dscb, sizeof(DSCBUFFERDESC));
		dscb.dwSize = sizeof(DSCBUFFERDESC);
		dscb.dwFlags = DSCBCAPS_CTRLFX;
		dscb.dwBufferBytes = m_size * m_count;
		dscb.dwReserved = 0;
		dscb.lpwfxFormat = &m_waveFMT;
		dscb.dwFXCount = 2;
		dscb.lpDSCFXDesc = &dsceds[0];

		DSBUFFERDESC dsb;
		ZeroMemory(&dsb, sizeof(DSBUFFERDESC));
		dsb.dwSize = sizeof(DSBUFFERDESC);
		dsb.dwBufferBytes = m_size * m_count;
		dsb.lpwfxFormat = &m_waveFMT;
		dsb.dwReserved = 0;
		dsb.dwFlags = DSBCAPS_GETCURRENTPOSITION2
			| DSBCAPS_GLOBALFOCUS
			| DSBCAPS_CTRLPOSITIONNOTIFY
			| DSBCAPS_LOCSOFTWARE
			| DSBCAPS_CTRLFREQUENCY;
		TinyComPtr<IDirectSoundCaptureFXAec8> aec;
		TinyComPtr<IDirectSoundCaptureFXNoiseSuppress8> ns;
		TinyComPtr<IDirectSoundNotify>	notify;
		TinyScopedArray<DSBPOSITIONNOTIFY> notifys(new DSBPOSITIONNOTIFY[m_count]);
		ASSERT(notifys);
		ZeroMemory(notifys, sizeof(DSBPOSITIONNOTIFY) * m_count);
		HRESULT hRes = S_OK;
		hRes = DirectSoundFullDuplexCreate8(&m_captureID, &m_renderID, &dscb, &dsb, m_hWND, DSSCL_PRIORITY, &m_dsfd, &m_dscb8, &m_dsb8, NULL);
		if (FAILED(hRes))
		{
			dscb.dwFlags = 0;
			dscb.dwFXCount = 0;
			dscb.lpDSCFXDesc = NULL;
			hRes = DirectSoundFullDuplexCreate8(&m_captureID, &m_renderID, &dscb, &dsb, m_hWND, DSSCL_PRIORITY, &m_dsfd, &m_dscb8, &m_dsb8, NULL);
			if (FAILED(hRes))
			{
				dsb.dwFlags &= ~DSBCAPS_CTRLFREQUENCY;
				hRes = DirectSoundFullDuplexCreate8(&m_captureID, &m_renderID, &dscb, &dsb, m_hWND, DSSCL_PRIORITY, &m_dsfd, &m_dscb8, &m_dsb8, NULL);
				if (FAILED(hRes))
				{
					HandleError(hRes);
					goto _ERROR;
				}
			}
		}
		hRes = m_dscb8->GetObjectInPath(GUID_DSCFX_CLASS_AEC, 0, IID_IDirectSoundCaptureFXAec8, (LPVOID*)&aec);
		if (SUCCEEDED(hRes))
		{
			DSCFXAec aecMode = { TRUE,TRUE,DSCFX_AEC_MODE_FULL_DUPLEX };
			aec->SetAllParameters(&aecMode);
		}
		hRes = m_dscb8->GetObjectInPath(GUID_DSCFX_CLASS_NS, 0, IID_IDirectSoundCaptureFXNoiseSuppress8, (LPVOID*)&ns);
		if (SUCCEEDED(hRes))
		{
			DSCFXNoiseSuppress nsMode = { TRUE };
			ns->SetAllParameters(&nsMode);
		}
		m_bits.Reset(new CHAR[m_size * m_count]);
		ASSERT(m_bits);
		hRes = m_dscb8->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		for (UINT32 i = 0; i < m_count; ++i)
		{
			notifys[i].dwOffset = (i + 1) * m_size - 1;
			notifys[i].hEventNotify = m_event;
		}
		hRes = notify->SetNotificationPositions(m_count, notifys);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		m_state = PCM_READY;
		return TRUE;
	_ERROR:
		m_state = PCM_NONE;
		return FALSE;
	}


	BOOL DSFullDuplexAudioInputStream::Start(AudioOutputCallback* callback)
	{
		if (m_state != PCM_READY)
			return FALSE;
		m_callback = callback;
		m_state = PCM_RECORDING;
		HRESULT hRes = S_OK;
		if (!m_event.ResetEvent())
		{
			HandleError(::GetLastError());
			return FALSE;
		}
		m_waiter.Unregister();
		if (!m_waiter.Register(m_event, INFINITE, BindCallback(&DSFullDuplexAudioInputStream::OnCallback, this)))
		{
			HandleError(::GetLastError());
			return FALSE;
		}
		hRes = m_dscb8->Start(DSCBSTART_LOOPING);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		return TRUE;
	}

	BOOL DSFullDuplexAudioInputStream::Stop()
	{
		if (m_state != PCM_RECORDING)
			return FALSE;
		m_dscb8->Stop();
		m_callback = NULL;
		m_state = PCM_READY;
		return TRUE;
	}

	void DSFullDuplexAudioInputStream::Close()
	{
		HRESULT hRes = S_OK;
		Stop();
		m_event.SetEvent();
		m_waiter.Unregister();
		m_dsfd.Release();
		m_dscb8.Release();
		m_state = PCM_CLOSED;
	}

	BOOL DSFullDuplexAudioInputStream::SetVolume(FLOAT volume)
	{
		return TRUE;
	}

	BOOL DSFullDuplexAudioInputStream::GetVolume(FLOAT* volume)
	{
		return TRUE;
	}

}