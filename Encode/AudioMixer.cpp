#include "stdafx.h"
#include "AudioMixer.h"

namespace Encode
{
	AudioMixer::AudioMixer()
	{
	}

	AudioMixer::~AudioMixer()
	{
	}
	void AudioMixer::Mix(BYTE* src, BYTE* dst, LONG size, BYTE* ps)
	{
		static INT const MAX = 32767;
		static INT const MIN = -32768;
		INT output = 0;
		DOUBLE ratio = 1;
		for (INT i = 0;i < size / 2;i++)
		{
			INT val = 0;
			val += *(SHORT*)(src + i * 2);
			val += *(SHORT*)(dst + i * 2);
			output = (INT)(val * ratio);
			if (output > MAX)
			{
				ratio = (DOUBLE)MAX / (DOUBLE)(output);
				output = MAX;
			}
			if (output < MIN)
			{
				ratio = (DOUBLE)MIN / (DOUBLE)(output);
				output = MIN;
			}
			if (ratio < 1)
			{
				ratio += ((DOUBLE)1 - ratio) / (DOUBLE)32;
			}
			*(SHORT*)(ps + i * 2) = (SHORT)output;
		}
	}
}

