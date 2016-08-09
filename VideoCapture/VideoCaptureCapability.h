#pragma once
#include "VideoCaptureParam.h"

namespace Media
{
	class VideoCaptureCapability
	{
	public:
		explicit VideoCaptureCapability(INT streamIndex);
		~VideoCaptureCapability();
	public:
		INT					StreamIndex;
		INT					RateNumerator;
		INT					RateDenominator;
		VideoCaptureParam	Parameter;
	};
}

