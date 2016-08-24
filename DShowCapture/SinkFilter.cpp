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

	HRESULT SinkFilter::SetMediaType(const AM_MEDIA_TYPE* mediaType)
	{
		ASSERT(m_sinkInputPin);
		return m_sinkInputPin->SetMediaType(mediaType);
	}
}
