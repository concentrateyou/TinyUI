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
#define AUDIO_BUFFERS (MAX_AUDIO_BUFFERS / 2)

		class TinySoundPlayer
		{
		public:
			TinySoundPlayer();
			virtual ~TinySoundPlayer();
		public:
			BOOL	Initialize(HWND hWND, WAVEFORMATEX* wfx);
			BOOL	Play(BYTE* bits, INT size);
			BOOL	GetCaps(DSCAPS& caps);
			BOOL	SetVolume(LONG volume);
			BOOL	GetVolume(LONG& volume);
			BOOL	SetFrequency(DWORD dwFrequency);
			BOOL	GetFrequency(DWORD& dwFrequency);
			BOOL	SetPlan(LONG plan);
			BOOL	GetPlan(LONG& plan);
			BOOL	Stop();
			BOOL	Close();
			WAVEFORMATEX* GetFormat();
		private:
			TinyComPtr<IDirectSound8>		m_sound;
			TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
			TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
			DSBPOSITIONNOTIFY				m_notifys[AUDIO_BUFFERS];
			HANDLE							m_events[AUDIO_BUFFERS];
			WAVEFORMATEX					m_waveFMT;
		};
	}
}

