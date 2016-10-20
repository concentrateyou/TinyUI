#include "stdafx.h"
#include "VideoSinkFilter.h"
#include "VideoSinkInputPin.h"

namespace Media
{

	VideoSinkFilter::VideoSinkFilter(FilterObserver* observer,TinyLock* lock)
		:FilterBase(__uuidof(VideoSinkFilter), lock)
	{
		m_sinkInputPin = new VideoSinkInputPin(this, observer);
	}

	VideoSinkFilter::~VideoSinkFilter()
	{

	}
	INT VideoSinkFilter::GetPinCount()
	{
		return 1;
	}

	IPin* VideoSinkFilter::GetPin(INT index)
	{
		return index == 0 ? m_sinkInputPin : NULL;
	}

	void VideoSinkFilter::SetCaptureParam(const VideoCaptureParam& param)
	{
		ASSERT(m_sinkInputPin);
		m_sinkInputPin->SetCaptureParam(param);
	}
}
