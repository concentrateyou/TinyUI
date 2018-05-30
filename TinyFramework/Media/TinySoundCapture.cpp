#include "..\stdafx.h"
#include "TinySoundCapture.h"
#include "TinyWASAPIAudio.h"

namespace TinyFramework
{
	namespace Media
	{
		TinySoundCapture::TinySoundCapture()
			:m_dwOffset(0),
			m_dwCount(0),
			m_dwSize(0)
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
				BOOL bIsMA = FALSE;
				if (TinyWASAPIAudio::IsMicrophone(eCapture, device.Guid, bIsMA) && bIsMA)
				{
					ps->push_back(device);
				}
			}
			return TRUE;
		}
		BOOL TinySoundCapture::GetCaps(const GUID& guid, DWORD& dwFormats)
		{
			TinyComPtr<IDirectSoundCapture8> dsc8;
			HRESULT hRes = DirectSoundCaptureCreate8(&guid, &dsc8, NULL);
			if (FAILED(hRes))
				return FALSE;
			DSCCAPS caps = { 0 };
			caps.dwSize = sizeof(DSCCAPS);
			hRes = dsc8->GetCaps(&caps);
			if (FAILED(hRes))
				return FALSE;
			dwFormats = caps.dwFormats;
			dsc8.Release();
			return TRUE;
		}
		BOOL TinySoundCapture::SetNotifys(DWORD dwCount, LPCDSBPOSITIONNOTIFY pNotify)
		{
			if (!m_dscb8)
				return FALSE;
			TinyComPtr<IDirectSoundNotify>	notify;
			HRESULT hRes = m_dscb8->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
			if (hRes != S_OK)
				return FALSE;
			if (SUCCEEDED(notify->SetNotificationPositions(dwCount, pNotify)))
			{
				m_dwCount = dwCount;
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinySoundCapture::GetBuffer(BYTE*& bits, LONG& size, DWORD dwFlag)
		{
			if (!m_dscb8)
				return FALSE;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			if (dwFlag != DSCBLOCK_ENTIREBUFFER)
			{
				DWORD	dwCapturePos = 0;
				DWORD	dwReadPos = 0;
				if (FAILED(m_dscb8->GetCurrentPosition(&dwCapturePos, &dwReadPos)))
					return FALSE;
				size = dwReadPos - m_dwOffset;
				if (size < 0)
					size += m_dwSize;
				if (size == 0)
					return FALSE;
				if (SUCCEEDED(m_dscb8->Lock(m_dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, dwFlag)))
				{
					size = dwAudioBytes;
					memcpy(m_bits, ppvAudioPtr, dwAudioBytes);
					bits = m_bits;
					m_dscb8->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
					m_dwOffset += dwAudioBytes;
					m_dwOffset %= m_dwSize;
					return TRUE;
				}
			}
			else
			{
				if (SUCCEEDED(m_dscb8->Lock(m_dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, dwFlag)))
				{
					size = dwAudioBytes;
					memcpy(m_bits, ppvAudioPtr, dwAudioBytes);
					bits = m_bits;
					m_dscb8->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
					return TRUE;
				}
			}
			return FALSE;
		}
		BOOL TinySoundCapture::SetFormat(const  WAVEFORMATEX* pFMT, DWORD dwSize)
		{
			if (!pFMT)
				return FALSE;
			m_dwSize = dwSize;
			m_bits.Reset(new BYTE[m_dwSize]);
			if (!m_bits)
				return FALSE;
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
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
			dscbdesc.dwBufferBytes = dwSize;
			dscbdesc.dwReserved = 0;
			dscbdesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			dscbdesc.dwFXCount = 2;
			dscbdesc.lpDSCFXDesc = dsceds;
			TinyComPtr<IDirectSoundCaptureBuffer> dscb;
			HRESULT hRes = m_dsc8->CreateCaptureBuffer(&dscbdesc, &dscb, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = dscb->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&m_dscb8);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundCapture::Open(const GUID& guid)
		{
			HRESULT hRes = DirectSoundCaptureCreate8(&guid, &m_dsc8, NULL);
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
		DWORD TinySoundCapture::GetSize() const
		{
			return m_dwSize;
		}
		//////////////////////////////////////////////////////////////////////////
		TinySoundCaptureAEC::TinySoundCaptureAEC()
		{

		}
		TinySoundCaptureAEC::~TinySoundCaptureAEC()
		{

		}
		BOOL TinySoundCaptureAEC::Open(LPGUID captureGUID, LPGUID renderGUID, const WAVEFORMATEX* pFMT, DWORD dwSize)
		{
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
			dscbdesc.dwBufferBytes = dwSize;
			dscbdesc.dwReserved = 0;
			dscbdesc.lpwfxFormat = const_cast<WAVEFORMATEX*>(pFMT);
			dscbdesc.dwFXCount = 2;
			dscbdesc.lpDSCFXDesc = dsceds;
			DSBUFFERDESC dsbdesc;
			memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
			dsbdesc.dwSize = sizeof(DSBUFFERDESC);
			dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
			dsbdesc.dwBufferBytes = dwSize;
			dsbdesc.lpwfxFormat = const_cast<WAVEFORMATEX*>(pFMT);
			HRESULT hRes = DirectSoundFullDuplexCreate8(captureGUID, renderGUID, &dscbdesc, &dsbdesc, GetDesktopWindow(), DSSCL_PRIORITY, &m_dsduplex, &m_dscb8, &m_dsb8, NULL);
			if (hRes != S_OK)
				return FALSE;

			return TRUE;
		}
	}
}