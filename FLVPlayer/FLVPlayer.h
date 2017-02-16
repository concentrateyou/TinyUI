#pragma once
#include "resource.h"
#include <concurrent_queue.h>
using namespace std;

namespace FLVPlayer
{
	typedef struct tagAVPacket
	{
		BYTE*	bits;
		INT		size;
		INT		type;
		INT		pts;
		INT		dts;
		INT		index;
		INT		duration;
	}AVPacket;

}

