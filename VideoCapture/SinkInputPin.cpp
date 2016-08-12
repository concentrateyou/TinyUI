#include "stdafx.h"
#include "SinkInputPin.h"

namespace Media
{
	const REFERENCE_TIME SecondsToReferenceTime = 10000000;
	SinkInputPin::SinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	SinkInputPin::~SinkInputPin()
	{

	}

	HRESULT SinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* mediaType)
	{
		return S_OK;
	}

	HRESULT SinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* mediaType)
	{
		return S_OK;
	}
}