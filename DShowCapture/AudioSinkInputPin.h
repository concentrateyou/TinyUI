#pragma once
#include "InputPinBase.h"
#include "FilterObserver.h"
#include "AudioCaptureParam.h"

namespace DShow
{
	class AudioSinkInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(AudioSinkInputPin)
	public:
		AudioSinkInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~AudioSinkInputPin();
		const AudioCaptureFormat& GetResponseFormat();
		void	SetRequestFormat(const AudioCaptureFormat& request);
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	private:
		AudioCaptureFormat	m_request;
		AudioCaptureFormat	m_response;
	};
}

