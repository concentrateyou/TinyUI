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
		BOOL Initialize(DWORD dstType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN, DWORD srcType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE);
		BOOL Close();
		BOOL SetMute(BOOL bMute);
		BOOL GetMute(BOOL& bMute);
		BOOL GetVolume(DWORD& volume);
		BOOL SetVolume(DWORD volume);
	private:
		HMIXER			m_hMixer;
		MIXERLINE		m_ml;
		MIXERCONTROL	m_mclMute;
		MIXERCONTROL	m_mclVolume;
	};
}

