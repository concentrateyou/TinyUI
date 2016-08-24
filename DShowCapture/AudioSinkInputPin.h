#pragma once
#include "InputPinBase.h"
#include "FilterObserver.h"
#include "AudioCaptureParam.h"

namespace Media
{
	class AudioSinkInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(AudioSinkInputPin);
	public:
		AudioSinkInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~AudioSinkInputPin();
		void SetCaptureParam(const AudioCaptureParam& param);
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	private:
		AudioCaptureParam	m_param;
	};
}

