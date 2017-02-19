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
			BOOL	Initialize(HWND hWND, WAVEFORMATEX* pFMT, DWORD dwSize = 0, DWORD dwCount = AUDIO_BUFFERS);
			BOOL	Play(BYTE* bits, INT size);
			BOOL	GetCaps(DSCAPS& caps);
			BOOL    IsPlaying();
			BOOL	Stop();
			BOOL	Reset();
			void	Close();
			WAVEFORMATEX* GetFormat();
		private:
			HRESULT Restore(LPDIRECTSOUNDBUFFER8 pDSB, BOOL* lost);
			LPDIRECTSOUNDBUFFER8 GetFree();
		private:
			TinyComPtr<IDirectSound8>		m_sound;
			TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
			LPDIRECTSOUNDBUFFER8*			m_secondarys;
			DWORD							m_dwSize;//缓冲大小
			DWORD							m_dwCount;//缓冲个数
			WAVEFORMATEX					m_waveFMT;
		};
	}
}

