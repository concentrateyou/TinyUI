#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class AudioBus
	{
	public:
		AudioBus();
		~AudioBus();
	private:
		INT m_frames;
	};
}
