#pragma once
#include "PinBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"

namespace Media
{
	class SinkInputPin : public PinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(SinkInputPin);
	public:
		SinkInputPin(IBaseFilter* filter, FilterObserver* observer);
		~SinkInputPin();
		void SetRequestedParam(const VideoCaptureParam& param);
		BOOL IsMediaTypeValid(const AM_MEDIA_TYPE* mediaType) OVERRIDE;
		BOOL GetValidMediaType(INT index, AM_MEDIA_TYPE* mediaType) OVERRIDE;
		HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample) OVERRIDE;
		const VideoCaptureParam& GetResultingParam();
	private:
		VideoCaptureParam	m_requesting;
		VideoCaptureParam	m_resulting;
		FilterObserver*		m_observer;
	};
}

