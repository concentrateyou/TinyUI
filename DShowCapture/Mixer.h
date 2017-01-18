#pragma once
#include "DShowCommon.h"
#include "AudioCapture.h"

namespace DShow
{
	class Mixer
	{
	public:
		Mixer();
		~Mixer();
		BOOL Initialize(DWORD dstType, DWORD srcType);
		BOOL Close();
		BOOL SetMute(BOOL bMute);
		BOOL GetMute(BOOL& bMute);
		BOOL GetVolume(DWORD& volume);
		BOOL SetVolume(DWORD volume);
	private:
		HMIXER			m_hMixer;
		MIXERLINE		m_ml;
	};
}

