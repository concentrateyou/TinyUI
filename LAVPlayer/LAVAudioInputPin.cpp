#include "stdafx.h"
#include "LAVAudioInputPin.h"

namespace LAV
{
	LAVAudioInputPin::LAVAudioInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, L"LAV Audio", observer)
	{
	}
	LAVAudioInputPin::~LAVAudioInputPin()
	{
	}
	HRESULT LAVAudioInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Audio)
			return E_INVALIDARG;
		return NOERROR;
	}
	HRESULT AudioSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		return NOERROR;
	}
}