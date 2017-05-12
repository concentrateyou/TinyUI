#pragma once
#include "TinyWave.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")

namespace TinyUI
{
	namespace Media
	{
		class TinySoundPlayer
		{
		public:
			TinySoundPlayer();
			virtual ~TinySoundPlayer();
		public:
			BOOL	Initialize(HWND hWND, WAVEFORMATEX* pFMT,DWORD dwSize);
			BOOL	SetPositions(DWORD dwSize, LPCDSBPOSITIONNOTIFY pNotify);
			BOOL	Play(DWORD dwFlags = DSBPLAY_LOOPING);
			BOOL	Fill(BYTE* bits, INT size);
			BOOL	GetCaps(DSCAPS& caps);
			BOOL	SetVolume(LONG volume);
			BOOL	GetVolume(LONG& volume);
			BOOL	SetFrequency(DWORD dwFrequency);
			BOOL	GetFrequency(DWORD& dwFrequency);
			BOOL	SetPlan(LONG plan);
			BOOL	GetPlan(LONG& plan);
			BOOL	Stop();
			BOOL	Close();
		private:
			TinyComPtr<IDirectSound8>		m_sound;
			TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
			TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
			WAVEFORMATEX*					m_waveFMT;
			DWORD							m_dwSize;
			DWORD							m_dwOffset;
		};
	}
}

