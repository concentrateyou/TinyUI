#include "stdafx.h"
#include "LAVAudioFilter.h"

namespace LAV
{
	LAVAudioFilter::LAVAudioFilter(FilterObserver* observer)
		:FilterBase(__uuidof(LAVAudioFilter), observer)
	{
		m_inputPin = new LAVAudioInputPin(this, observer);
	}

	LAVAudioFilter::~LAVAudioFilter()
	{
	}
	INT LAVAudioFilter::GetPinCount()
	{
		return 1;
	}

	IPin* LAVAudioFilter::GetPin(INT index)
	{
		return index == 0 ? m_inputPin : NULL;
	}
}