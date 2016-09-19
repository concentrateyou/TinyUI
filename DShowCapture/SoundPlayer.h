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
		BOOL	Initialize(HWND hWND,const WAVEFORMATEX& wfx);
		BOOL	GetCaps(DSCAPS& caps);
		BOOL	Play(BYTE* bits, INT size);
		BOOL	Stop();
	private:
		TinyComPtr<IDirectSound8>		m_sound;
		TinyComPtr<IDirectSoundBuffer8>	m_buffer;
		TinyComPtr<IDirectSoundNotify8>	m_notify;
	};
}

