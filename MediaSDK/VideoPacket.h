#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class VideoPacket
	{
	public:
		VideoPacket();
		~VideoPacket();
	};

	struct tagVideoSample
	{
		INT64 timestamp;

	}VideoSample;
}