#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
#include "Media/TinyMedia.h"
using namespace TinyFramework;
using namespace TinyFramework::Media;

namespace Encode
{
	/// <summary>
	/// PCM混音目前支持两路
	/// </summary>
	class AudioMixer
	{
		DISALLOW_COPY_AND_ASSIGN(AudioMixer)
	public:
		AudioMixer();
		~AudioMixer();
	public:
		static void Mix(BYTE* src, BYTE* dst, LONG size, BYTE* ps);
	};
}


