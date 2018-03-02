#pragma once
#include "Common.h"
#include "RTMPStream.h"
#include "HTTPStream.h"
#include "AACDecoder.h"
#include "x264Decoder.h"
#include <vector>

namespace Decode
{
	class MP4Writer
	{
		DISALLOW_COPY_AND_ASSIGN(MP4Writer)
	public:
		MP4Writer();
		virtual ~MP4Writer();
	};

}

