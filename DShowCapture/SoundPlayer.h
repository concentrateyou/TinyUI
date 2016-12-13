#pragma once
#include "DShowCommon.h"
#include "Media/TinyWave.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")
using namespace TinyUI;
using namespace TinyUI::Media;

namespace DShow
{
	class SoundPlayer
	{
	public:
		SoundPlayer();
		virtual ~SoundPlayer();
	public:
		BOOL	LoadFile(LPSTR szFilename);
		BOOL	LoadStream(LPVOID pStream, DWORD bufferSize);
		BOOL	GetCaps(DSCAPS& caps);
		BOOL	Initialize(HWND hWND, WAVEFORMATEX* wfx);
		BOOL	Play(BYTE* bits, INT size);
		BOOL	Stop();
		BOOL	Close();
	private:
		TinyWaveFile					m_waveFile;
		TinyComPtr<IDirectSound8>		m_sound;
		TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
		TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
		DWORD							m_dwOffset;
		WAVEFORMATEX*					m_waveFMT;
	};
}

