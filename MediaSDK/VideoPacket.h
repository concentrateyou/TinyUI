#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	typedef struct tagVideoPacket
	{
		INT64				timestamp;
		LONG				size;
		BYTE*				data[MAX_AV_PLANES];
		UINT32				linesize[MAX_AV_PLANES];
		UINT32				x;
		UINT32				y;
		UINT32				cx;
		UINT32				cy;
		UINT32				format;
	}VideoPacket;
}