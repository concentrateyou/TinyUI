#pragma once
#include "PinBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"

namespace Media
{
	class SinkInputPin : public PinBase, public IMemInputPin
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(SinkInputPin);
	public:
		SinkInputPin(FilterBase* pFilter, FilterObserver* observer);
		~SinkInputPin();
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* mediaType) OVERRIDE;
		HRESULT GetMediaType(INT index, AM_MEDIA_TYPE* mediaType) OVERRIDE;
		HRESULT SetMediaType(const AM_MEDIA_TYPE *mediaType) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample) OVERRIDE;
	private:
		FilterObserver*		m_observer;
		
	};
}

