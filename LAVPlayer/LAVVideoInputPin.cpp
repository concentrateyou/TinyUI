#include "stdafx.h"
#include "LAVVideoInputPin.h"
#include "LAVVideo.h"

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
		ASSERT(m_observer);
		if (position != 0)
			return S_FALSE;
		if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return S_FALSE;
		LAVVideo* ps = reinterpret_cast<LAVVideo*>(m_observer);
		if (ps != NULL)
		{
			ScopedMediaType mediaType;
			if (!ps->GetOutputMediaType(mediaType.Receive()))
				return E_FAIL;
			if (mediaType->formattype == FORMAT_VideoInfo &&
				mediaType->subtype == MEDIASUBTYPE_RGB32)
			{
				VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
				memcpy(pvi, mediaType->pbFormat, sizeof(VIDEOINFOHEADER));
				pMediaType->majortype = MEDIATYPE_Video;
				pMediaType->formattype = FORMAT_VideoInfo;
				pMediaType->bTemporalCompression = FALSE;
				pMediaType->subtype = MEDIASUBTYPE_RGB32;
				pMediaType->bFixedSizeSamples = TRUE;
				pMediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
			}
		}
		return NOERROR;
	}
}