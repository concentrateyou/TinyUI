#include "stdafx.h"
#include "LAVVideoFilter.h"

namespace LAV
{
	LAVVideoFilter::LAVVideoFilter(FilterObserver* observer)
		:FilterBase(__uuidof(LAVVideoFilter), observer)
	{
		m_inputPin = new LAVVideoInputPin(this, observer);
	}

	LAVVideoFilter::~LAVVideoFilter()
	{
	}

	INT LAVVideoFilter::GetPinCount()
	{
		return 1;
	}

	IPin* LAVVideoFilter::GetPin(INT index)
	{
		return index == 0 ? m_inputPin : NULL;
	}

	const LAVVideoFormat& LAVVideoFilter::GetResponseFormat()
	{
		ASSERT(m_inputPin);
		return	m_inputPin->GetResponseFormat();
	}
	void LAVVideoFilter::SetRequestFormat(const LAVVideoFormat& request)
	{
		ASSERT(m_inputPin);
		m_inputPin->SetRequestFormat(request);
	}
}