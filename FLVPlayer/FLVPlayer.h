#pragma once
#include "resource.h"
#include <concurrent_queue.h>
using namespace std;

namespace FLVPlayer
{
	typedef struct tagAVPacket
	{
		BYTE*	bits;
		LONG	size;
		LONG	pts;
		LONG	dts;
		LONG	index;
		LONG	duration;
	}AVPacket;

	class AVPacketQueue
	{
	private:
		concurrency::concurrent_queue<AVPacket>	m_packets;
	};

}

