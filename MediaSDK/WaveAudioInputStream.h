#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	class WaveAudioInputStream : public AudioInputStream
	{
		DISALLOW_COPY_AND_ASSIGN(WaveAudioInputStream)
	public:
		WaveAudioInputStream();
		virtual	~WaveAudioInputStream();
	};
}


