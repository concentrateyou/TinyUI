#include "stdafx.h"
#include "VideoSinkFilter.h"
#include "VideoSinkInputPin.h"

namespace DShow
{

	VideoSinkFilter::VideoSinkFilter(FilterObserver* observer)
		:FilterBase(__uuidof(VideoSinkFilter), observer)
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
	const VideoCaptureFormat& VideoSinkFilter::GetResponseFormat()
	{
		ASSERT(m_sinkInputPin);
		return m_sinkInputPin->GetResponseFormat();
	}
	void VideoSinkFilter::SetRequestFormat(const VideoCaptureFormat& request)
	{
		ASSERT(m_sinkInputPin);
		m_sinkInputPin->SetRequestFormat(request);
	}
}
