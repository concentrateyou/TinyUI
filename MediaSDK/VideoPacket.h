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

	struct VideoSample
	{
		VideoPixelFormat	format;
		INT64				timestamp;
		BYTE*				data[MAX_AV_PLANES];
		UINT32				linesize[MAX_AV_PLANES];
		UINT32				cx;
		UINT32				cy;
	};
}