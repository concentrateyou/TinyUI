#pragma once
#include "InputPinBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"

namespace DShow
{
	class VideoSinkInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(VideoSinkInputPin)
	public:
		VideoSinkInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~VideoSinkInputPin();
		void SetCaptureParam(const VideoCaptureParam& param);
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	private:
		VideoCaptureParam	m_param;
	};
}

