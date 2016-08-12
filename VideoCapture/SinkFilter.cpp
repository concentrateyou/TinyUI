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

	IPin* SinkFilter::GetPin(int index)
	{
		return index == 0 ? m_sinkInputPin : NULL;
	}

	void SinkFilter::SetRequestedParam(const VideoCaptureParam& param)
	{
		ASSERT(m_sinkInputPin);
		return m_sinkInputPin->SetRequestedParam(param);
	}
	const VideoCaptureParam& SinkFilter::GetResultingParam()
	{
		ASSERT(m_sinkInputPin);
		return m_sinkInputPin->GetResultingParam();
	}
	HRESULT SinkFilter::SetMediaType(const AM_MEDIA_TYPE* mediaType)
	{
		ASSERT(m_sinkInputPin);
		return m_sinkInputPin->SetMediaType(mediaType);
	}
}
