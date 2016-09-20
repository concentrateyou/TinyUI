#include "stdafx.h"
#include "SoundPlayer.h"
#pragma comment(lib,"dsound.lib")

namespace Media
{
	SoundPlayer::SoundPlayer()
		:m_dwOffset(0)
	{
	}

	SoundPlayer::~SoundPlayer()
	{
	}

	BOOL SoundPlayer::Initialize(HWND hWND, const WAVEFORMATEX& wfx)
	{
		m_wfx = wfx;
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
		dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primaryDSB, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_primaryDSB->SetFormat(&m_wfx);
		if (FAILED(hRes))
			return FALSE;
		//…Ë÷√æ≤Ã¨ª∫≥Â«¯
		ZeroMemory(&dbdesc, sizeof(dbdesc));
		dbdesc.dwSize = sizeof(dbdesc);
		dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS | DSBCAPS_GLOBALFOCUS;
		dbdesc.dwBufferBytes = 3 * m_wfx.nAvgBytesPerSec;//3√Î
		dbdesc.lpwfxFormat = &m_wfx;
		TinyComPtr<IDirectSoundBuffer> dsb;
		hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryDSB);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL SoundPlayer::GetCaps(DSCAPS& caps)
	{
		ASSERT(m_sound);
		caps.dwSize = sizeof(DSCAPS);
		return m_sound->GetCaps(&caps) == S_OK;
	}
	BOOL SoundPlayer::Play(BYTE* bits, INT size)
	{
		ASSERT(m_secondaryDSB);
		HRESULT hRes = S_OK;
		hRes = m_secondaryDSB->SetCurrentPosition(0);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_secondaryDSB->Play(0, 0, 0);
		if (FAILED(hRes))
			return FALSE;
		LPVOID	ppvAudioPtr = NULL;
		DWORD	dwAudioBytes = 0;
		hRes = m_secondaryDSB->Lock(0, 0, &ppvAudioPtr, &dwAudioBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
		if (FAILED(hRes))
			return FALSE;
		memcpy(ppvAudioPtr, bits, size);
		m_secondaryDSB->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
		return TRUE;
	}
	BOOL SoundPlayer::Stop()
	{
		ASSERT(m_secondaryDSB);
		HRESULT hRes = S_OK;
		hRes = m_secondaryDSB->Restore();
		if (FAILED(hRes))
			return FALSE;
		return m_secondaryDSB->Stop() == S_OK;
	}
}