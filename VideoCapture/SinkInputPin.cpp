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
	HRESULT SinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* mediaType)
	{
		GUID type = mediaType->majortype;
		if (type != MEDIATYPE_Video)
			return S_FALSE;
		GUID formatType = mediaType->formattype;
		if (formatType != FORMAT_VideoInfo)
			return S_FALSE;
		GUID subType = mediaType->subtype;
		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
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
	HRESULT SinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* mediaType)
	{
		if (position != 0)
			return FALSE;

		if (mediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return S_FALSE;
		if (!mediaType->pbFormat)
		{
			mediaType->pbFormat = (BYTE *)CoTaskMemAlloc(sizeof(VIDEOINFOHEADER));
		}
		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biClrImportant = 0;
		pvi->bmiHeader.biClrUsed = 0;
		if (m_param.GetRate() > 0)
		{
			pvi->AvgTimePerFrame = static_cast<REFERENCE_TIME>(SecondsToReferenceTime / m_param.GetRate());
		}
		mediaType->majortype = MEDIATYPE_Video;
		mediaType->formattype = FORMAT_VideoInfo;
		mediaType->bTemporalCompression = FALSE;
		switch (m_param.GetFormat())
		{
		case PIXEL_FORMAT_RGB24:
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 24;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_param.GetSize().cx * m_param.GetSize().cy) * 3;
			mediaType->subtype = MEDIASUBTYPE_RGB24;
			break;
		case PIXEL_FORMAT_MJPEG:
			pvi->bmiHeader.biCompression = MAKEFOURCC('M', 'J', 'P', 'G');
			pvi->bmiHeader.biBitCount = 24;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_param.GetSize().cx * m_param.GetSize().cy) * 3;
			mediaType->subtype = MEDIASUBTYPE_MJPG;
			break;
		case PIXEL_FORMAT_I420:
			pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
			pvi->bmiHeader.biBitCount = 12;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_param.GetSize().cx * m_param.GetSize().cy) * 3 / 2;
			mediaType->subtype = MediaSubTypeI420;
			break;
		case PIXEL_FORMAT_YUY2:
			pvi->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
			pvi->bmiHeader.biBitCount = 16;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_param.GetSize().cx * m_param.GetSize().cy) * 2;
			mediaType->subtype = MEDIASUBTYPE_YUY2;
			break;
		default:
			return S_FALSE;
		}
		mediaType->bFixedSizeSamples = TRUE;
		mediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
		return S_OK;
	}
	void SinkInputPin::SetParam(const VideoCaptureParam& param)
	{
		m_param = param;
	}
}