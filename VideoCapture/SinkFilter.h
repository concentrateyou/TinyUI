#pragma once
#include "FilterBase.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"
#include "SinkInputPin.h"

namespace Media
{
	class __declspec(uuid("88cdbbdc-a73b-4afa-acbf-15d5e2ce12c3"))
	SinkFilter : public FilterBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(SinkFilter);
	public:
		explicit SinkFilter(FilterObserver* observer);
		virtual ~SinkFilter();
		HRESULT SetMediaType(const AM_MEDIA_TYPE* mediaType);
		void SetParam(const VideoCaptureParam& param);
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
	private:
		TinyScopedReferencePtr<SinkInputPin> m_sinkInputPin;
	};
}