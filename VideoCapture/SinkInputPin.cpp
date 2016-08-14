#include "stdafx.h"
#include "SinkInputPin.h"

namespace Media
{
	SinkInputPin::SinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	SinkInputPin::~SinkInputPin()
	{
	}
	HRESULT SinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Video)
			return S_FALSE;
		GUID formatType = pMediaType->formattype;
		if (formatType != FORMAT_VideoInfo)
			return S_FALSE;
		GUID subType = pMediaType->subtype;
		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		if (pvi == NULL)
			return S_FALSE;
		if (subType == MediaSubTypeI420 &&pvi->bmiHeader.biCompression == MAKEFOURCC('I', '4', '2', '0'))
			return NOERROR;
		if (subType == MEDIASUBTYPE_MJPG &&pvi->bmiHeader.biCompression == MAKEFOURCC('M', 'J', 'P', 'G'))
			return NOERROR;
		if (subType == MEDIASUBTYPE_YUY2 &&pvi->bmiHeader.biCompression == MAKEFOURCC('Y', 'U', 'Y', '2'))
			return NOERROR;
		if (subType == MEDIASUBTYPE_RGB24 && pvi->bmiHeader.biCompression == BI_RGB)
			return NOERROR;
		return S_FALSE;
	}
	HRESULT SinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return FALSE;
		if (pMediaType != &m_mediaType)
		{
			FreeMediaType(*pMediaType);
			CopyMediaType(pMediaType,&m_mediaType);
		}
		return S_OK;
	}
}