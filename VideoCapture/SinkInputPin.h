#pragma once
#include "PinBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"

namespace Media
{
	class SinkInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(SinkInputPin);
	public:
		SinkInputPin(FilterBase* pFilter, FilterObserver* observer);
		~SinkInputPin();
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* mediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* mediaType) OVERRIDE;
	};
}

