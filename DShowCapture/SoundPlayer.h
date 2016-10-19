#pragma once
#include "DShowCommon.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
using namespace TinyUI;

namespace Media
{
	class SoundPlayer
	{
	public:
		SoundPlayer();
		~SoundPlayer();
	public:
		BOOL	Initialize(HWND hWND, const WAVEFORMATEX& wfx);
		BOOL	GetCaps(DSCAPS& caps);
		BOOL	Play(BYTE* bits, INT size);
		BOOL	Stop();
		BOOL	Close();
	private:
		TinyComPtr<IDirectSound8>		m_sound;
		TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
		TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
		DWORD							m_dwOffset;
		WAVEFORMATEX					m_wfx;
	};
}

