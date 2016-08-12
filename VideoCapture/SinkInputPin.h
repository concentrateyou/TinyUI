#pragma once
#include "InputPinBase.h"
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
		void SetRequestedParam(const VideoCaptureParam& param);
		const VideoCaptureParam& SinkInputPin::GetResultingParam();
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* mediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* mediaType) OVERRIDE;
	private:
		VideoCaptureParam	m_requesting;
		VideoCaptureParam	m_resulting;
	};
}

