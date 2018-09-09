#include "stdafx.h"
#include "VideoSinkInputPin.h"

namespace DShow
{
	const REFERENCE_TIME SecondsToReferenceTime = 10000000;

	VideoPixelFormat GetPixelFormat(const GUID& subType)
	{
		static struct
		{
			const GUID& subType;
			VideoPixelFormat format;
		}
		formats[] =
		{
			{ MediaSubTypeI420, PIXEL_FORMAT_I420 },
			{ MEDIASUBTYPE_IYUV, PIXEL_FORMAT_I420 },
			{ MEDIASUBTYPE_RGB24, PIXEL_FORMAT_RGB24 },
			{ MEDIASUBTYPE_RGB32, PIXEL_FORMAT_RGB32 },
			{ MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2 },
			{ MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG },
			{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY }
		};
		for (size_t i = 0; i < ARRAYSIZE_UNSAFE(formats); ++i)
		{
			if (subType == formats[i].subType)
				return formats[i].format;
		}
		return PIXEL_FORMAT_UNKNOWN;
	}

	VideoSinkInputPin::VideoSinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	VideoSinkInputPin::~VideoSinkInputPin()
	{
	}
	void VideoSinkInputPin::SetRequestFormat(const VideoCaptureFormat& request)
	{
		m_request = request;
		m_response.SetSize(0, 0);
		m_response.SetRate(0.0F);
		m_response.SetFormat(PIXEL_FORMAT_UNKNOWN);
	}
	const VideoCaptureFormat& VideoSinkInputPin::GetResponseFormat()
	{
		return m_response;
	}
	HRESULT VideoSinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Video)
			return E_INVALIDARG;
		GUID formatType = pMediaType->formattype;
		if (formatType != FORMAT_VideoInfo)
			return E_INVALIDARG;
		GUID subType = pMediaType->subtype;
		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		if (pvi == NULL)
			return E_INVALIDARG;
		m_response.SetSize(pvi->bmiHeader.biWidth, abs(pvi->bmiHeader.biHeight));
		if (pvi->AvgTimePerFrame > 0)
		{
			m_response.SetRate(static_cast<FLOAT>(SecondsToReferenceTime / pvi->AvgTimePerFrame));
		}
		else
		{
			m_response.SetRate(m_request.GetRate());
		}
		if (subType == MEDIASUBTYPE_RGB24 &&pvi->bmiHeader.biCompression == BI_RGB)
		{
			m_response.SetFormat(PIXEL_FORMAT_RGB24);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_RGB32 &&pvi->bmiHeader.biCompression == BI_RGB)
		{
			m_response.SetFormat(PIXEL_FORMAT_RGB32);
			return NOERROR;
		}
		if (subType == MediaSubTypeI420 &&	pvi->bmiHeader.biCompression == MAKEFOURCC('I', '4', '2', '0'))
		{
			m_response.SetFormat(PIXEL_FORMAT_I420);
			return NOERROR;
		}
		if (subType == MEDIASUBTYPE_YUY2 &&	pvi->bmiHeader.biCompression == MAKEFOURCC('Y', 'U', 'Y', '2'))
		{
			m_response.SetFormat(PIXEL_FORMAT_YUY2);
			return NOERROR;
		}
		return E_FAIL;
	}
	HRESULT VideoSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return E_FAIL;

		if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return E_FAIL;

		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biClrImportant = 0;
		pvi->bmiHeader.biClrUsed = 0;
		if (m_request.GetRate() > 0)
		{
			pvi->AvgTimePerFrame = static_cast<REFERENCE_TIME>(SecondsToReferenceTime / m_request.GetRate());
		}
		pMediaType->majortype = MEDIATYPE_Video;
		pMediaType->formattype = FORMAT_VideoInfo;
		pMediaType->bTemporalCompression = FALSE;
		switch (m_request.GetFormat())
		{
		case PIXEL_FORMAT_I420:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
			pvi->bmiHeader.biBitCount = 12;
			pvi->bmiHeader.biWidth = m_request.GetSize().cx;
			pvi->bmiHeader.biHeight = m_request.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_request.GetSize().cx * m_request.GetSize().cy * 3 / 2;
			pMediaType->subtype = MediaSubTypeI420;
			break;
		}
		case PIXEL_FORMAT_YUY2:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
			pvi->bmiHeader.biBitCount = 16;
			pvi->bmiHeader.biWidth = m_request.GetSize().cx;
			pvi->bmiHeader.biHeight = m_request.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_request.GetSize().cx * m_request.GetSize().cy * 2;
			pMediaType->subtype = MEDIASUBTYPE_YUY2;
			break;
		}
		case PIXEL_FORMAT_RGB24:
		{
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 24;
			pvi->bmiHeader.biWidth = m_request.GetSize().cx;
			pvi->bmiHeader.biHeight = m_request.GetSize().cy;
			pvi->bmiHeader.biSizeImage = LINESIZE(24, pvi->bmiHeader.biWidth) * pvi->bmiHeader.biHeight;
			pMediaType->subtype = MEDIASUBTYPE_RGB24;
			break;
		}
		case PIXEL_FORMAT_RGB32:
		{
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 32;
			pvi->bmiHeader.biWidth = m_request.GetSize().cx;
			pvi->bmiHeader.biHeight = m_request.GetSize().cy;
			pvi->bmiHeader.biSizeImage = LINESIZE(32, pvi->bmiHeader.biWidth) * pvi->bmiHeader.biHeight;
			pMediaType->subtype = MEDIASUBTYPE_RGB32;
			break;
		}
		default:
			return E_FAIL;
		}
		pMediaType->bFixedSizeSamples = TRUE;
		pMediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
		return NOERROR;
	}
}