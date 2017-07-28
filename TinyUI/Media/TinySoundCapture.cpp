#include "..\stdafx.h"
#include "TinySoundCapture.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundCapture::TinySoundCapture()
			:m_dwOffset(0)
		{

		}
		TinySoundCapture::~TinySoundCapture()
		{
		}
		BOOL TinySoundCapture::Enumerate(std::vector<CAPTUREDEVICE>& devices)
		{
			HRESULT hRes = DirectSoundCaptureEnumerate(TinySoundCapture::DSEnumCallback, &devices);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL CALLBACK TinySoundCapture::DSEnumCallback(LPGUID pzGUID, LPCSTR pzDesc, LPCSTR pzModule, LPVOID pContext)
		{
			if (pzGUID != NULL)
			{
				std::vector<CAPTUREDEVICE>* ps = static_cast<std::vector<CAPTUREDEVICE>*>(pContext);
				CAPTUREDEVICE device = { 0 };
				device.Guid = *pzGUID;
				device.Description = pzDesc;
				device.Module = pzModule;
				ps->push_back(device);
			}
			return TRUE;
		}
		BOOL TinySoundCapture::GetCaps(const GUID& guid, DWORD& dwFormats)
		{
			m_dsc8.Release();
			HRESULT hRes = DirectSoundCaptureCreate8(&guid, &m_dsc8, NULL);
			if (FAILED(hRes))
				return FALSE;
			DSCCAPS caps = { 0 };
			caps.dwSize = sizeof(DSCCAPS);
			hRes = m_dsc8->GetCaps(&caps);
			if (FAILED(hRes))
				return FALSE;
			dwFormats = caps.dwFormats;
			m_dsc8.Release();
			return TRUE;
		}
		BOOL TinySoundCapture::SetNotifys(DWORD dwSize, LPCDSBPOSITIONNOTIFY pNotify)
		{
			if (!m_dscb8)
				return FALSE;
			TinyComPtr<IDirectSoundNotify>	notify;
			HRESULT hRes = m_dscb8->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
			if (hRes != S_OK)
				return FALSE;
			return SUCCEEDED(notify->SetNotificationPositions(dwSize, pNotify));
		}
		BOOL TinySoundCapture::Fill(BYTE* bits, LONG size, DWORD dwOffset)
		{
			if (!m_dscb8)
				return FALSE;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			HRESULT hRes = m_dscb8->Lock(dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (SUCCEEDED(hRes))
			{
				memcpy(ppvAudioPtr, bits, size);
				m_dscb8->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			}
			return TRUE;
		}
		BOOL TinySoundCapture::Open(const GUID& guid, WAVEFORMATEX* pFMT)
		{
			HRESULT hRes = DirectSoundCaptureCreate8(&guid, &m_dsc8, NULL);
			if (FAILED(hRes))
				return FALSE;
			if (pFMT != NULL)
			{
				m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
				memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			}
			else
			{
				m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
				WAVEFORMATEX* ps = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
				ps->cbSize = 0;
				ps->nChannels = 2;
				ps->wBitsPerSample = 16;
				ps->nSamplesPerSec = 44100;
				ps->wFormatTag = WAVE_FORMAT_PCM;
				ps->nBlockAlign = 4;
				ps->nAvgBytesPerSec = 176400;
			}
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
			DSCBUFFERDESC dscbdesc = { 0 };
			dscbdesc.dwSize = sizeof(DSCBUFFERDESC);
			dscbdesc.dwFlags = DSCBCAPS_CTRLFX;
			dscbdesc.dwBufferBytes = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr())->nAvgBytesPerSec;
			dscbdesc.dwReserved = 0;
			dscbdesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			dscbdesc.dwFXCount = 2;
			dscbdesc.lpDSCFXDesc = dsceds;
			TinyComPtr<IDirectSoundCaptureBuffer> dscb;
			hRes = m_dsc8->CreateCaptureBuffer(&dscbdesc, &dscb, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = dscb->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&m_dscb8);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundCapture::Start(DWORD dwFlags)
		{
			if (!m_dscb8)
				return FALSE;
			return SUCCEEDED(m_dscb8->Start(dwFlags));
		}
		BOOL TinySoundCapture::Stop()
		{
			if (!m_dscb8)
				return FALSE;
			return SUCCEEDED(m_dscb8->Stop());
		}
		BOOL TinySoundCapture::Close()
		{
			if (m_dscb8)
			{
				m_dscb8->Stop();
			}
			m_dscb8.Release();
			m_dsc8.Release();
			m_dwOffset = 0;
			return TRUE;
		}
	}
}