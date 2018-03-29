#include "stdafx.h"
#include "LAVVideoFilter.h"

namespace LAV
{
	LAVVideoFilter::LAVVideoFilter()
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
}