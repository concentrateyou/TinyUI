#pragma once
#include "DShowCommon.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")
using namespace TinyUI;

namespace DShow
{
	class SoundPlayer
	{
	public:
		SoundPlayer();
		virtual ~SoundPlayer();
	public:
		BOOL	Initialize(HWND hWND, WAVEFORMATEX* wfx);
		BOOL	GetCaps(DSCAPS& caps);
		BOOL	Play(BYTE* bits, INT size);
		BOOL	Stop();
		BOOL	Close();
	private:
		TinyComPtr<IDirectSound8>		m_sound;
		TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
		TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
		DWORD							m_dwOffset;
		WAVEFORMATEX					m_waveFMT;
	};
}

