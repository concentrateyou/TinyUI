#include "stdafx.h"
#include "AudioSinkInputPin.h"

namespace Media
{
	AudioSinkInputPin::AudioSinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	AudioSinkInputPin::~AudioSinkInputPin()
	{
	}
	void AudioSinkInputPin::SetCaptureParam(const VideoCaptureParam& param)
	{
		m_param = param;
	}
	HRESULT AudioSinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Audio)
			return S_FALSE;
		GUID formatType = pMediaType->formattype;
		if (formatType != FORMAT_WaveFormatEx)
			return S_FALSE;
		GUID subType = pMediaType->subtype;
		WAVEFORMATEX* pvi = reinterpret_cast<WAVEFORMATEX*>(pMediaType->pbFormat);
		if (pvi == NULL)
			return S_FALSE;
		return S_FALSE;
	}
	HRESULT AudioSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return S_FALSE;
		if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return S_FALSE;
		return NOERROR;
	}
}