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
		BOOL TinySoundPlayer::Initialize(HWND hWND, WAVEFORMATEX* pFMT, DWORD dwSize, DWORD dwCount)
		{
			m_dwCount = dwCount;
			m_dwSize = dwSize == 0 ? pFMT->nAvgBytesPerSec : dwSize;
			m_waveFMT = *pFMT;
			HRESULT hRes = S_OK;
			hRes = DirectSoundCreate8(NULL, &m_sound, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_sound->SetCooperativeLevel(hWND, DSSCL_PRIORITY);
			if (FAILED(hRes))
				return FALSE;
			//…Ë÷√÷˜ª∫≥Â
			DSBUFFERDESC dbdesc = { 0 };
			dbdesc.dwSize = sizeof(dbdesc);
			dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primaryDSB, NULL);
			if (FAILED(hRes))
				goto DS_ERROR;
			hRes = m_primaryDSB->SetFormat(pFMT);
			if (FAILED(hRes))
				goto DS_ERROR;
			//…Ë÷√∏®÷˙ª∫≥Â«¯
			m_secondarys = new LPDIRECTSOUNDBUFFER8[dwCount];
			if (!m_secondarys)
				goto DS_ERROR;
			for (DWORD i = 0;i < dwCount;i++)
			{
				ZeroMemory(&dbdesc, sizeof(dbdesc));
				dbdesc.dwSize = sizeof(dbdesc);
				dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;
				dbdesc.dwBufferBytes = m_dwSize;
				dbdesc.lpwfxFormat = pFMT;
				TinyComPtr<IDirectSoundBuffer> dsb;
				hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
				if (FAILED(hRes))
					goto DS_ERROR;
				hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondarys[i]);
				if (FAILED(hRes))
					goto DS_ERROR;
				hRes = Restore(m_secondarys[i], NULL);
				if (FAILED(hRes))
					goto DS_ERROR;
			}
			return TRUE;
		DS_ERROR:
			m_sound.Release();
			m_primaryDSB.Release();
			return FALSE;
		}
		WAVEFORMATEX* TinySoundPlayer::GetFormat()
		{
			return &m_waveFMT;
		}
		BOOL TinySoundPlayer::GetCaps(DSCAPS& caps)
		{
			ASSERT(m_sound);
			caps.dwSize = sizeof(DSCAPS);
			return m_sound->GetCaps(&caps) == S_OK;
		}
		BOOL TinySoundPlayer::Play(BYTE* bits, INT size)
		{
			if (m_secondarys == NULL)
				return FALSE;
			LPDIRECTSOUNDBUFFER8 pDSB = GetFree();
			if (pDSB == NULL)
				return FALSE;
			HRESULT hRes = S_OK;
			LPVOID	ppvAudioPtr = NULL;
			DWORD	dwAudioBytes = 0;
			hRes = pDSB->Lock(0, 0, &ppvAudioPtr, &dwAudioBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
			if (FAILED(hRes))
			{
				if (hRes != DSERR_BUFFERLOST)
					return FALSE;
				hRes = pDSB->Restore();
				if (FAILED(hRes))
					return FALSE;
				hRes = pDSB->Lock(0, 0, &ppvAudioPtr, &dwAudioBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
				if (FAILED(hRes))
					return FALSE;
			}
			memcpy(ppvAudioPtr, bits, size);
			pDSB->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
			hRes = pDSB->SetCurrentPosition(0);
			if (FAILED(hRes))
				return FALSE;
			hRes = pDSB->Play(0, 0, 0);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinySoundPlayer::IsPlaying()
		{
			if (m_secondarys == NULL)
				return FALSE;
			BOOL bIsPlaying = FALSE;
			for (DWORD i = 0; i < m_dwCount; i++)
			{
				if (m_secondarys[i])
				{
					DWORD dwStatus = 0;
					m_secondarys[i]->GetStatus(&dwStatus);
					bIsPlaying |= ((dwStatus & DSBSTATUS_PLAYING) != 0);
				}
			}
			return bIsPlaying;
		}
		BOOL TinySoundPlayer::Reset()
		{
			if (m_secondarys == NULL)
				return FALSE;
			HRESULT hRes = 0;
			for (DWORD i = 0; i < m_dwCount; i++)
				hRes |= m_secondarys[i]->SetCurrentPosition(0);
			return hRes;
		}
		BOOL TinySoundPlayer::Stop()
		{
			HRESULT hRes = S_OK;
			for (DWORD i = 0;i < m_dwCount;i++)
				hRes |= m_secondarys[i]->Stop();
			return hRes == S_OK;
		}
		void TinySoundPlayer::Close()
		{
			m_primaryDSB.Release();
			for (DWORD i = 0;i < m_dwCount;i++)
			{
				m_secondarys[i]->Release();
			}
			SAFE_DELETE_ARRAY(m_secondarys);
		}
		LPDIRECTSOUNDBUFFER8 TinySoundPlayer::GetFree()
		{
			if (m_secondarys == NULL)
				return FALSE;
			DWORD i = 0;
			for (i = 0; i < m_dwCount; i++)
			{
				if (m_secondarys[i])
				{
					DWORD dwStatus = 0;
					m_secondarys[i]->GetStatus(&dwStatus);
					if ((dwStatus & DSBSTATUS_PLAYING) == 0)
						break;
				}
			}
			if (i != m_dwCount)
				return m_secondarys[i];
			else
				return m_secondarys[rand() % m_dwCount];
		}
		HRESULT TinySoundPlayer::Restore(LPDIRECTSOUNDBUFFER8 pDSB, BOOL* lost)
		{
			HRESULT hRes;
			if (pDSB == NULL)
				return CO_E_NOTINITIALIZED;
			if (lost)
				*lost = FALSE;
			DWORD dwStatus;
			if (FAILED(hRes = pDSB->GetStatus(&dwStatus)))
				return hRes;
			if (dwStatus & DSBSTATUS_BUFFERLOST)
			{
				do
				{
					hRes = pDSB->Restore();
					if (hRes == DSERR_BUFFERLOST)
					{
						Sleep(10);
					}
				} while ((hRes = pDSB->Restore()) == DSERR_BUFFERLOST);
				if (lost != NULL)
					*lost = TRUE;
				return S_OK;
			}
			return S_FALSE;
		}
	}
}