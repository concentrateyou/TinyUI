#pragma once
#include "FilterBase.h"
#include "FilterObserver.h"
#include "AudioCaptureParam.h"
#include "AudioSinkInputPin.h"

namespace DShow
{
	class __declspec(uuid("{F5997302-B10C-4AE9-8933-B24484950594}"))
		AudioSinkFilter : public FilterBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(AudioSinkFilter);
	public:
		explicit AudioSinkFilter(FilterObserver* observer);
		virtual ~AudioSinkFilter();
		void SetCaptureParam(const AudioCaptureParam& param);
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
	private:
		TinyScopedReferencePtr<AudioSinkInputPin> m_sinkInputPin;
		AudioCaptureParam	m_param;
	};
}

