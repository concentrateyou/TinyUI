#include "stdafx.h"
#include "DSAudioInputStream.h"

namespace MediaSDK
{
	DSAudioInputStream::DSAudioInputStream()
		:m_count(0),
		m_size(0),
		m_dwOffset(0),
		m_state(PCM_NONE),
		m_callback(NULL)
	{
		m_event.CreateEvent();
	}

	DSAudioInputStream::~DSAudioInputStream()
	{
	}

	BOOL DSAudioInputStream::Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& dID)
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
		m_dID = dID;
		m_count = count;
		m_params = params;
		m_state = PCM_NONE;
		m_size = ((m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8) * m_params.GetFrames();
		m_buffer.Initialize(m_count, m_size);
		m_packet.Reset(new AudioPacket(params));
		ASSERT(m_packet);
		return TRUE;
	}

	void DSAudioInputStream::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "DSAudioInputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}

	void DSAudioInputStream::OnCallback(BOOLEAN timerFired)
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
			LONG blocksize = static_cast<LONG>(dwReadPos - m_dwOffset);
			if (blocksize < 0)
			{
				blocksize += static_cast<LONG>(m_size * m_count);
			}
			if (blocksize == 0)
			{
				break;
			}
			hRes = m_dscb8->Lock(m_dwOffset, static_cast<DWORD>(blocksize), &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				break;
			}
			if (m_callback != NULL)
			{
				DWORD dwWhrite = m_buffer.Write(ppvAudioPtr, dwAudioBytes);
				DWORD dwRead = m_buffer.Read(m_packet->data(), m_size);
				TRACE("dwWhrite:%d, dwRead:%d\n", dwWhrite, dwRead);
				m_callback->OnOutput(TinyPerformanceTime::Now(), m_packet);
			}
			hRes = m_dscb8->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				break;
			}
			m_dwOffset += dwAudioBytes;
			m_dwOffset %= (m_size * m_count);
		} while (0);
	}

	BOOL DSAudioInputStream::Open()
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
		DSCBUFFERDESC dscbdesc;
		ZeroMemory(&dscbdesc, sizeof(DSCBUFFERDESC));
		dscbdesc.dwSize = sizeof(DSCBUFFERDESC);
		dscbdesc.dwFlags = DSCBCAPS_CTRLFX;
		dscbdesc.dwBufferBytes = m_size * m_count;
		dscbdesc.dwReserved = 0;
		dscbdesc.lpwfxFormat = &m_waveFMT;
		dscbdesc.dwFXCount = 2;
		dscbdesc.lpDSCFXDesc = dsceds;
		TinyComPtr<IDirectSoundNotify>	notify;
		TinyScopedArray<DSBPOSITIONNOTIFY> notifys(new DSBPOSITIONNOTIFY[m_count]);
		ASSERT(notifys);
		ZeroMemory(notifys, sizeof(DSBPOSITIONNOTIFY) * m_count);
		HRESULT hRes = S_OK;
		hRes = DirectSoundCaptureCreate8(&m_dID, &m_dsc8, NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_dsc8->GetCaps(&caps);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_dsc8->CreateCaptureBuffer(&dscbdesc, &m_dscb, NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_dscb->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&m_dscb8);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
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


	BOOL DSAudioInputStream::Start(AudioOutputCallback* callback)
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
		if (!m_waiter.Register(m_event, INFINITE, BindCallback(&DSAudioInputStream::OnCallback, this)))
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

	BOOL DSAudioInputStream::Stop()
	{
		if (m_state != PCM_RECORDING)
			return FALSE;
		m_dscb8->Stop();
		m_callback = NULL;
		m_state = PCM_READY;
		return TRUE;
	}

	void DSAudioInputStream::Close()
	{
		HRESULT hRes = S_OK;
		Stop();
		m_event.SetEvent();
		m_waiter.Unregister();
		m_dsc8.Release();
		m_dscb8.Release();
		m_state = PCM_CLOSED;
	}

	BOOL DSAudioInputStream::SetVolume(DOUBLE volume)
	{
		return TRUE;
	}

	BOOL DSAudioInputStream::GetVolume(DOUBLE* volume)
	{
		return TRUE;
	}

}