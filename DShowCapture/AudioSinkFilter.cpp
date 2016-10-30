#include "stdafx.h"
#include "AudioSinkFilter.h"
#include "AudioSinkInputPin.h"

namespace DShow
{
	AudioSinkFilter::AudioSinkFilter(FilterObserver* observer)
		:FilterBase(__uuidof(AudioSinkFilter), observer)
	{
		m_sinkInputPin = new AudioSinkInputPin(this, observer);
	}

	AudioSinkFilter::~AudioSinkFilter()
	{

	}
	INT AudioSinkFilter::GetPinCount()
	{
		return 1;
	}

	IPin* AudioSinkFilter::GetPin(INT index)
	{
		return index == 0 ? m_sinkInputPin : NULL;
	}

	void AudioSinkFilter::SetCaptureParam(const AudioCaptureParam& param)
	{
		ASSERT(m_sinkInputPin);
		m_sinkInputPin->SetCaptureParam(param);
	}
}
