#include "stdafx.h"
#include "SinkFilter.h"
#include "SinkInputPin.h"

namespace Media
{

	SinkFilter::SinkFilter(FilterObserver* observer)
		:FilterBase(__uuidof(SinkFilter), FILTER_NAME)
	{
		m_sinkInputPin = new SinkInputPin(this, observer);
	}

	SinkFilter::~SinkFilter()
	{

	}
	INT SinkFilter::GetPinCount()
	{
		return 1;
	}

	IPin* SinkFilter::GetPin(INT index)
	{
		return index == 0 ? m_sinkInputPin : NULL;
	}

	void SinkFilter::SetCaptureParam(const VideoCaptureParam& param)
	{
		ASSERT(m_sinkInputPin);
		m_sinkInputPin->SetCaptureParam(param);
	}
}
