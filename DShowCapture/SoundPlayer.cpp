#include "stdafx.h"
#include "SoundPlayer.h"
#pragma comment(lib,"dsound.lib")

namespace Media
{
	SoundPlayer::SoundPlayer()
	{
	}

	SoundPlayer::~SoundPlayer()
	{
	}

	BOOL SoundPlayer::Initialize(HWND hWND, const WAVEFORMATEX& wfx)
	{
		HRESULT hRes = S_OK;
		hRes = DirectSoundCreate8(NULL, &m_sound, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_sound->Initialize(NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_sound->SetCooperativeLevel(hWND, DSSCL_PRIORITY);
		if (FAILED(hRes))
			return FALSE;
		DSBUFFERDESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY;
		desc.dwBufferBytes = 3 * wfx.nAvgBytesPerSec;
		desc.lpwfxFormat = &wfx;
		TinyComPtr<IDirectSoundBuffer> dsbPrimary;
		hRes = m_sound->CreateSoundBuffer(&desc, &dsbPrimary, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = dsbPrimary->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_buffer);
		if (FAILED(hRes))
			return FALSE;
		m_buffer->SetCurrentPosition(0);
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
		ASSERT(m_buffer);
		HRESULT hRes = S_OK;
		hRes = m_buffer->Play(0, 0, DSBPLAY_LOOPING);
		if (FAILED(hRes))
			return FALSE;
		LPVOID	ppvAudioPtr1 = NULL;
		DWORD	dwAudioBytes1 = 0;
		LPVOID	ppvAudioPtr2 = NULL;
		DWORD	dwAudioBytes2 = 0;
		hRes = m_buffer->Lock(0, 0, &ppvAudioPtr1, &dwAudioBytes1, &ppvAudioPtr2, &dwAudioBytes2, 0);
		if (FAILED(hRes))
			return FALSE;

		m_buffer->Unlock(ppvAudioPtr1, dwAudioBytes1, ppvAudioPtr2, dwAudioBytes2);
		return TRUE;
	}
	BOOL SoundPlayer::Stop()
	{

	}
}