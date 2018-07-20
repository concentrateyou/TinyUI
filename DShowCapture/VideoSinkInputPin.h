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
		const VideoCaptureFormat& GetResponseFormat();
		void	SetRequestFormat(const VideoCaptureFormat& request);
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	private:
		VideoCaptureFormat	m_request;
		VideoCaptureFormat	m_response;
	};
}

