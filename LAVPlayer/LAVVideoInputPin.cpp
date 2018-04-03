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
		LAVVideo* ps = reinterpret_cast<LAVVideo*>(m_observer);
		if (ps != NULL)
		{
			TinyArray<ScopedMediaType> mediaTypes;
			if (!ps->GetOutputMediaTypes(mediaTypes))
				return E_FAIL;
			for (INT i = 0;i < mediaTypes.GetSize();i++)
			{
				AM_MEDIA_TYPE* mediaType = mediaTypes[i].Ptr();
				if ((mediaType->formattype == FORMAT_VideoInfo2 || mediaType->formattype == FORMAT_VideoInfo) &&
					mediaType->subtype == MEDIASUBTYPE_RGB32)
				{
					return NOERROR;
				}
			}
		}
		return E_FAIL;
	}
	HRESULT LAVVideoInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return E_FAIL;
		if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return E_FAIL;
		LAVVideo* ps = reinterpret_cast<LAVVideo*>(m_observer);
		if (ps != NULL)
		{
			TinyArray<ScopedMediaType> mediaTypes;
			if (!ps->GetOutputMediaTypes(mediaTypes))
				return E_FAIL;
			for (INT i = 0;i < mediaTypes.GetSize();i++)
			{
				AM_MEDIA_TYPE* mediaType = mediaTypes[i].Ptr();
				if (mediaType->formattype == FORMAT_VideoInfo2)
				{
					VIDEOINFOHEADER2* s = reinterpret_cast<VIDEOINFOHEADER2*>(mediaType->pbFormat);
					VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
					ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
					pvi->AvgTimePerFrame = s->AvgTimePerFrame;
					pvi->dwBitErrorRate = s->dwBitErrorRate;
					pvi->dwBitRate = s->dwBitRate;
					pvi->rcSource = s->rcSource;
					pvi->rcTarget = s->rcTarget;
					memcpy(&pvi->bmiHeader, &s->bmiHeader, sizeof(BITMAPINFOHEADER));
					pMediaType->majortype = MEDIATYPE_Video;
					pMediaType->formattype = FORMAT_VideoInfo;
					pMediaType->bTemporalCompression = FALSE;
					pMediaType->subtype = MEDIASUBTYPE_RGB32;
					pMediaType->bFixedSizeSamples = TRUE;
					pMediaType->lSampleSize = pvi->bmiHeader.biWidth * pvi->bmiHeader.biHeight * 4;
				}
				if (mediaType->formattype == FORMAT_VideoInfo)
				{
					VIDEOINFOHEADER* s = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
					VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
					memcpy_s(pvi, sizeof(VIDEOINFOHEADER), s, sizeof(VIDEOINFOHEADER));
				}
				return NOERROR;
			}
		}
		return E_FAIL;
	}
}