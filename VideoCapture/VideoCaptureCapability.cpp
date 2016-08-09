#include "stdafx.h"
#include "VideoCaptureCapability.h"

namespace Media
{
	VideoCaptureCapability::VideoCaptureCapability(INT streamIndex)
		:StreamIndex(streamIndex),
		RateNumerator(0),
		RateDenominator(1)
	{
	}
	VideoCaptureCapability::~VideoCaptureCapability()
	{
	}
}
