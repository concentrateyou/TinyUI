#include "..\stdafx.h"
#include "TinySoundPlayer.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundPlayer::TinySoundPlayer()
			:m_dwSize(0),
			m_dwOffset(0)
		{
		}

		TinySoundPlayer::~TinySoundPlayer()
		{
		}
		BOOL TinySoundPlayer::Initialize(HWND hWND)
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
		BOOL TinySoundPlayer::SetFormat(WAVEFORMATEX* pFMT, DWORD dwSize)
		{
			ASSERT(m_sound);
			m_dwSize = dwSize;
			m_waveFMT = pFMT;
			HRESULT hRes = S_OK;
			DSBUFFERDESC dbdesc;
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primaryDSB, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_primaryDSB->SetFormat(pFMT);
			if (hRes != S_OK)
				return FALSE;
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
			dbdesc.dwBufferBytes = m_dwSize;
			dbdesc.lpwfxFormat = pFMT;
			TinyComPtr<IDirectSoundBuffer> dsb;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryDSB);
			if (hRes != S_OK)
				return FALSE;
			TinyScopedArray<BYTE> bits(new BYTE[m_dwSize]);
			ZeroMemory(bits, m_dwSize);
			this->Fill(bits, m_dwSize);
			return TRUE;
		}
		BOOL TinySoundPlayer::GetCaps(DSCAPS& caps)
		{
			ASSERT(m_sound);
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
		BOOL TinySoundPlayer::Fill(BYTE* bits, INT size)
		{
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			HRESULT hRes = m_secondaryDSB->Lock(m_dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (hRes != S_OK)
			{
				if (hRes != DSERR_BUFFERLOST)
					return FALSE;
				hRes = m_secondaryDSB->Restore();
				if (hRes != S_OK)
					return FALSE;
				hRes = m_secondaryDSB->Lock(m_dwOffset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
				if (hRes != S_OK)
					return FALSE;
			}
			memcpy(ppvAudioPtr, bits, size);
			m_secondaryDSB->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			m_dwOffset += size;
			m_dwOffset %= m_dwSize;
			return TRUE;
		}
		BOOL TinySoundPlayer::Stop()
		{
			ASSERT(m_secondaryDSB);
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
				return hRes;
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
			return TRUE;
		}
		DWORD TinySoundPlayer::GetSize() const
		{
			return m_dwSize;
		}
		BOOL TinySoundPlayer::SetNotifys(DWORD dwSize, LPCDSBPOSITIONNOTIFY pNotify)
		{
			ASSERT(m_secondaryDSB);
			TinyComPtr<IDirectSoundNotify>	notify;
			HRESULT hRes = m_secondaryDSB->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
			if (hRes != S_OK)
				return FALSE;
			return SUCCEEDED(notify->SetNotificationPositions(dwSize, pNotify));
		}
	}
}