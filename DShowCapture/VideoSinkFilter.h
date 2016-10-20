#pragma once
#include "FilterBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"
#include "VideoSinkInputPin.h"

namespace Media
{
	class __declspec(uuid("88cdbbdc-a73b-4afa-acbf-15d5e2ce12c3"))
	VideoSinkFilter : public FilterBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(VideoSinkFilter);
	public:
		explicit VideoSinkFilter(FilterObserver* observer, TinyLock* lock);
		virtual ~VideoSinkFilter();
		void SetCaptureParam(const VideoCaptureParam& param);
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
	private:
		TinyScopedReferencePtr<VideoSinkInputPin> m_sinkInputPin;
		VideoCaptureParam	m_param;
	};
}
