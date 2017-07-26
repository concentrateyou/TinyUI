#include "..\stdafx.h"
#include "TinySoundCapture.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundCapture::TinySoundCapture()
			:m_dwOffset(0)
		{
			ZeroMemory(&m_sFMT, sizeof(m_sFMT));
			m_sFMT.cbSize = 0;
			m_sFMT.wFormatTag = WAVE_FORMAT_PCM;
			m_sFMT.nChannels = 2;
			m_sFMT.nSamplesPerSec = 44100;
			m_sFMT.wBitsPerSample = 16;
			m_sFMT.nBlockAlign = m_sFMT.nChannels * (m_sFMT.wBitsPerSample / 8);
			m_sFMT.nAvgBytesPerSec = m_sFMT.nSamplesPerSec * m_sFMT.nBlockAlign;
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
				memcpy(&m_sFMT, pFMT, sizeof(WAVEFORMATEX));
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
			dscbdesc.dwBufferBytes = m_sFMT.nAvgBytesPerSec;
			dscbdesc.dwReserved = 0;
			dscbdesc.lpwfxFormat = &m_sFMT;
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