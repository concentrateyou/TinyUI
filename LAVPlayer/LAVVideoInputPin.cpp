#include "stdafx.h"
#include "LAVVideoInputPin.h"

namespace LAV
{
	LAVVideoInputPin::LAVVideoInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, L"LAV Video", observer)
	{

	}
	LAVVideoInputPin::~LAVVideoInputPin()
	{

	}
	HRESULT LAVVideoInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Video)
			return E_INVALIDARG;

		return NOERROR;
	}
	HRESULT LAVVideoInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		return NOERROR;
	}
}