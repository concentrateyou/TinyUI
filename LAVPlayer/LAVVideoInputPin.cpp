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
	const LAVVideoFormat& LAVVideoInputPin::GetResponseFormat()
	{
		return m_response;
	}
	void LAVVideoInputPin::SetRequestFormat(const LAVVideoFormat& request)
	{
		m_request = request;
		m_response.SetSize(0, 0);
		m_response.SetFormat(PIXEL_FORMAT_UNKNOWN);
	}
	HRESULT LAVVideoInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Video)
			return E_INVALIDARG;
		GUID formatType = pMediaType->formattype;
		if (formatType != FORMAT_VideoInfo)
			return E_INVALIDARG;
		GUID subType = pMediaType->subtype;
		VIDEOINFOHEADER* s = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		m_response.SetSize(s->bmiHeader.biWidth, abs(s->bmiHeader.biHeight));
		if (subType == MEDIASUBTYPE_RGB32 &&s->bmiHeader.biCompression == BI_RGB)
		{
			m_response.SetFormat(PIXEL_FORMAT_RGB32);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_NV12 && s->bmiHeader.biCompression == MAKEFOURCC('N', 'V', '1', '2'))
		{
			m_response.SetFormat(PIXEL_FORMAT_NV12);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_YUY2 && s->bmiHeader.biCompression == MAKEFOURCC('Y', 'U', 'Y', '2'))
		{
			m_response.SetFormat(PIXEL_FORMAT_YUY2);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_UYVY && s->bmiHeader.biCompression == MAKEFOURCC('U', 'Y', 'V', 'Y'))
		{
			m_response.SetFormat(PIXEL_FORMAT_UYVY);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_I420 && s->bmiHeader.biCompression == MAKEFOURCC('I', '4', '2', '0'))
		{
			m_response.SetFormat(PIXEL_FORMAT_I420);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_YV12 && s->bmiHeader.biCompression == MAKEFOURCC('Y', 'V', '1', '2'))
		{
			m_response.SetFormat(PIXEL_FORMAT_YV12);
			return NOERROR;
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
			for (INT i = 0; i < mediaTypes.GetSize(); i++)
			{
				AM_MEDIA_TYPE* mediaType = mediaTypes[i].Ptr();
				if (mediaType->formattype == FORMAT_VideoInfo)
				{
					if ((mediaType->subtype == MEDIASUBTYPE_RGB32 && m_request.GetFormat() == PIXEL_FORMAT_RGB32) ||
						(mediaType->subtype == MEDIASUBTYPE_NV12 && m_request.GetFormat() == PIXEL_FORMAT_NV12) ||
						(mediaType->subtype == MEDIASUBTYPE_YUY2 && m_request.GetFormat() == PIXEL_FORMAT_YUY2) ||
						(mediaType->subtype == MEDIASUBTYPE_UYVY && m_request.GetFormat() == PIXEL_FORMAT_UYVY) ||
						(mediaType->subtype == MEDIASUBTYPE_I420 && m_request.GetFormat() == PIXEL_FORMAT_I420) ||
						(mediaType->subtype == MEDIASUBTYPE_YV12 && m_request.GetFormat() == PIXEL_FORMAT_YV12))
					{
						VIDEOINFOHEADER* s = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
						VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
						memcpy_s(pvi, sizeof(VIDEOINFOHEADER), s, sizeof(VIDEOINFOHEADER));
						return NOERROR;
					}
				}
			}
		}
		return E_FAIL;
	}
}