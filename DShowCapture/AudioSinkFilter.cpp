#include "stdafx.h"
#include "AudioSinkFilter.h"
#include "AudioSinkInputPin.h"

namespace Media
{
	AudioSinkFilter::AudioSinkFilter(FilterObserver* observer,TinyLock* lock)
		:FilterBase(__uuidof(AudioSinkFilter), lock)
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
