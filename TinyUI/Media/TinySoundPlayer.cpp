#include "..\stdafx.h"
#include "TinySoundPlayer.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundPlayer::TinySoundPlayer()
			:m_dwSize(0),
			m_dwOffset(0),
			m_dwCount(0)
		{
		}

		TinySoundPlayer::~TinySoundPlayer()
		{
		}
		BOOL TinySoundPlayer::Enumerate(std::vector<PLAYDEVICE>& devices)
		{
			HRESULT hRes = DirectSoundEnumerate(TinySoundPlayer::DSEnumCallback, &devices);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL CALLBACK TinySoundPlayer::DSEnumCallback(LPGUID pzGUID, LPCSTR pzDesc, LPCSTR pzModule, LPVOID pContext)
		{
			if (pzGUID != NULL)
			{
				std::vector<PLAYDEVICE>* ps = static_cast<std::vector<PLAYDEVICE>*>(pContext);
				PLAYDEVICE device = { 0 };
				device.Guid = *pzGUID;
				device.Description = pzDesc;
				device.Module = pzModule;
				ps->push_back(device);
			}
			return TRUE;
		}
		BOOL TinySoundPlayer::Open(const GUID& guid, HWND hWND)
		{
			HRESULT hRes = S_OK;
			hRes = DirectSoundCreate8(&guid, &m_sound, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_sound->SetCooperativeLevel(hWND, DSSCL_PRIORITY);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundPlayer::Open(HWND hWND)
		{
			HRESULT hRes = S_OK;
			hRes = DirectSoundCreate8(NULL, &m_sound, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_sound->SetCooperativeLevel(hWND, DSSCL_PRIORITY);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundPlayer::SetFormat(const WAVEFORMATEX* pFMT, DWORD dwSize)
		{
			if (!m_sound || !pFMT)
				return FALSE;
			INT size = sizeof(WAVEFORMATEX) + pFMT->cbSize;
			m_waveFMT.Reset(new BYTE[size]);
			memcpy(m_waveFMT, (BYTE*)pFMT, size);
			HRESULT hRes = S_OK;
			DSBUFFERDESC dbdesc;
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primaryDSB, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_primaryDSB->SetFormat(reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr()));
			if (hRes != S_OK)
				return FALSE;
			DSBCAPS dsbcaps = { 0 };
			dsbcaps.dwSize = sizeof(DSBCAPS);
			hRes = m_primaryDSB->GetCaps(&dsbcaps);
			if (hRes != S_OK)
				return FALSE;
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
			dbdesc.dwBufferBytes = dwSize;
			dbdesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			TinyComPtr<IDirectSoundBuffer> dsb;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryDSB);
			if (hRes != S_OK)
				return FALSE;
			m_dwSize = dwSize;
			return TRUE;
		}
		BOOL TinySoundPlayer::GetCaps(DSCAPS& caps)
		{
			if (!m_sound)
				return FALSE;
			caps.dwSize = sizeof(DSCAPS);
			return m_sound->GetCaps(&caps) == S_OK;
		}
		BOOL TinySoundPlayer::SetVolume(LONG volume)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->SetVolume(volume) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::GetVolume(LONG& volume)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->GetVolume(&volume) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::SetFrequency(DWORD dwFrequency)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->SetFrequency(dwFrequency) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::GetFrequency(DWORD& dwFrequency)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->GetFrequency(&dwFrequency) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::SetPlan(LONG plan)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->SetPan(plan) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::GetPlan(LONG& plan)
		{
			if (m_secondaryDSB)
				return m_secondaryDSB->GetPan(&plan) == S_OK;
			return TRUE;
		}
		BOOL TinySoundPlayer::Play(DWORD dwFlag)
		{
			if (!m_secondaryDSB)
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_secondaryDSB->SetCurrentPosition(0);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_secondaryDSB->Play(0, 0, dwFlag);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundPlayer::Fill(BYTE* bits, LONG size, DWORD dwOffset)
		{
			if (!m_secondaryDSB)
				return FALSE;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			HRESULT hRes = m_secondaryDSB->Lock(dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (hRes != S_OK)
			{
				if (hRes != DSERR_BUFFERLOST)
					return FALSE;
				hRes = m_secondaryDSB->Restore();
				if (hRes != S_OK)
					return FALSE;
				hRes = m_secondaryDSB->Lock(dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
				if (hRes != S_OK)
					return FALSE;
			}
			memcpy(ppvAudioPtr, bits, size);
			m_secondaryDSB->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			return TRUE;
		}
		BOOL TinySoundPlayer::GetPosition(DWORD& dwCurrentPlayCursor, DWORD& dwCurrentWriteCursor)
		{
			if (!m_secondaryDSB)
				return FALSE;
			HRESULT hRes = m_secondaryDSB->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor);
			return hRes == S_OK;
		}
		BOOL TinySoundPlayer::Stop()
		{
			if (!m_secondaryDSB)
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_secondaryDSB->Restore();
			if (hRes != S_OK)
				return FALSE;
			return m_secondaryDSB->Stop() == S_OK;
		}
		BOOL TinySoundPlayer::Close()
		{
			HRESULT hRes = S_OK;
			if (!m_secondaryDSB || !m_primaryDSB)
				return FALSE;
			hRes = m_secondaryDSB->Restore();
			if (hRes != S_OK)
				return FALSE;
			hRes = m_secondaryDSB->Stop();
			if (hRes != S_OK)
				return FALSE;
			hRes = m_primaryDSB->Restore();
			if (hRes != S_OK)
				return FALSE;
			hRes = m_primaryDSB->Stop();
			if (hRes != S_OK)
				return FALSE;
			m_secondaryDSB.Release();
			m_primaryDSB.Release();
			m_dwSize = 0;
			m_dwOffset = 0;
			return TRUE;
		}

		BOOL TinySoundPlayer::IsEmpty() const
		{
			return (m_sound == NULL || m_secondaryDSB == NULL || m_primaryDSB == NULL);
		}

		DWORD TinySoundPlayer::GetSize() const
		{
			return m_dwSize;
		}
		BOOL TinySoundPlayer::SetNotifys(DWORD dwCount, LPCDSBPOSITIONNOTIFY pNotify)
		{
			if (!m_secondaryDSB)
				return FALSE;
			TinyComPtr<IDirectSoundNotify>	notify;
			HRESULT hRes = m_secondaryDSB->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
			if (hRes != S_OK)
				return FALSE;
			if (SUCCEEDED(notify->SetNotificationPositions(dwCount, pNotify)))
			{
				m_dwCount = dwCount;
				return TRUE;
			}
			return FALSE;
		}
	}
}