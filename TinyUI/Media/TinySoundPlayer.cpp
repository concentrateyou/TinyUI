#include "..\stdafx.h"
#include "TinySoundPlayer.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundPlayer::TinySoundPlayer()
		{
		}

		TinySoundPlayer::~TinySoundPlayer()
		{
		}
		BOOL TinySoundPlayer::Initialize(HWND hWND, WAVEFORMATEX* pFMT)
		{
			m_waveFMT = pFMT;
			HRESULT hRes = S_OK;
			hRes = DirectSoundCreate8(NULL, &m_sound, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_sound->SetCooperativeLevel(hWND, DSSCL_PRIORITY);
			if (FAILED(hRes))
				return FALSE;
			//…Ë÷√÷˜ª∫≥Â
			DSBUFFERDESC dbdesc;
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primaryDSB, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_primaryDSB->SetFormat(pFMT);
			if (FAILED(hRes))
				return FALSE;
			//…Ë÷√æ≤Ã¨ª∫≥Â«¯
			ZeroMemory(&dbdesc, sizeof(dbdesc));
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS | DSBCAPS_GLOBALFOCUS;
			dbdesc.dwBufferBytes = 3 * pFMT->nAvgBytesPerSec;//3√Î
			dbdesc.lpwfxFormat = pFMT;
			TinyComPtr<IDirectSoundBuffer> dsb;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryDSB);
			if (FAILED(hRes))
				return FALSE;
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
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->SetVolume(volume) == S_OK;
		}
		BOOL TinySoundPlayer::GetVolume(LONG& volume)
		{
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->GetVolume(&volume) == S_OK;
		}
		BOOL TinySoundPlayer::SetFrequency(DWORD dwFrequency)
		{
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->SetFrequency(dwFrequency) == S_OK;
		}
		BOOL TinySoundPlayer::GetFrequency(DWORD& dwFrequency)
		{
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->GetFrequency(&dwFrequency) == S_OK;
		}
		BOOL TinySoundPlayer::SetPlan(LONG plan)
		{
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->SetPan(plan) == S_OK;
		}
		BOOL TinySoundPlayer::GetPlan(LONG& plan)
		{
			ASSERT(m_secondaryDSB);
			return m_secondaryDSB->GetPan(&plan) == S_OK;
		}
		BOOL TinySoundPlayer::Play(BYTE* bits, INT size)
		{
			ASSERT(m_secondaryDSB);
			HRESULT hRes = S_OK;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			hRes = m_secondaryDSB->Lock(0, 0, &ppvAudioPtr, &dwAudioBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
			if (FAILED(hRes))
			{
				if (hRes != DSERR_BUFFERLOST)
					return FALSE;
				hRes = m_secondaryDSB->Restore();
				if (FAILED(hRes))
					return FALSE;
				hRes = m_secondaryDSB->Lock(0, 0, &ppvAudioPtr, &dwAudioBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
				if (FAILED(hRes))
					return FALSE;
			}
			memcpy(ppvAudioPtr, bits, size);
			m_secondaryDSB->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			hRes = m_secondaryDSB->SetCurrentPosition(0);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_secondaryDSB->Play(0, 0, 0);
			if (FAILED(hRes))
				return FALSE;

			return TRUE;
		}
		BOOL TinySoundPlayer::Stop()
		{
			ASSERT(m_secondaryDSB);
			HRESULT hRes = S_OK;
			hRes = m_secondaryDSB->Restore();
			if (FAILED(hRes))
				return FALSE;
			return m_secondaryDSB->Stop() == S_OK;
		}
		BOOL TinySoundPlayer::Close()
		{
			HRESULT hRes = S_OK;
			if (!m_secondaryDSB || !m_primaryDSB)
				return hRes;
			hRes = m_secondaryDSB->Restore();
			if (FAILED(hRes))
				return FALSE;
			hRes = m_secondaryDSB->Stop();
			if (FAILED(hRes))
				return FALSE;
			hRes = m_primaryDSB->Restore();
			if (FAILED(hRes))
				return FALSE;
			hRes = m_primaryDSB->Stop();
			if (FAILED(hRes))
				return FALSE;
			m_secondaryDSB.Release();
			m_primaryDSB.Release();
			return TRUE;
		}
	}
}