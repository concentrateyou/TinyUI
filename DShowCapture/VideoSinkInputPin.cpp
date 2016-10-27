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
	void VideoSinkInputPin::SetCaptureParam(const VideoCaptureParam& param)
	{
		m_param = param;
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
		if (m_param.GetFormat() == GetPixelFormat(subType))
			return NOERROR;
		return S_FALSE;
	}
	HRESULT VideoSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return S_FALSE;

		if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return S_FALSE;

		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biClrImportant = 0;
		pvi->bmiHeader.biClrUsed = 0;
		if (m_param.GetRate() > 0)
		{
			pvi->AvgTimePerFrame = static_cast<REFERENCE_TIME>(SecondsToReferenceTime / m_param.GetRate());
		}
		pMediaType->majortype = MEDIATYPE_Video;
		pMediaType->formattype = FORMAT_VideoInfo;
		pMediaType->bTemporalCompression = FALSE;
		switch (m_param.GetFormat())
		{
		case PIXEL_FORMAT_I420:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
			pvi->bmiHeader.biBitCount = 12;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_param.GetSize().cx * m_param.GetSize().cy * 3 / 2;
			pMediaType->subtype = MediaSubTypeI420;
			break;
		}
		case PIXEL_FORMAT_YUY2:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
			pvi->bmiHeader.biBitCount = 16;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_param.GetSize().cx * m_param.GetSize().cy * 2;
			pMediaType->subtype = MEDIASUBTYPE_YUY2;
			break;
		}
		case PIXEL_FORMAT_RGB24:
		{
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 24;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_param.GetSize().cx * m_param.GetSize().cy * 3;
			pMediaType->subtype = MEDIASUBTYPE_RGB24;
			break;
		}
		case PIXEL_FORMAT_RGB32:
		{
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 32;
			pvi->bmiHeader.biWidth = m_param.GetSize().cx;
			pvi->bmiHeader.biHeight = m_param.GetSize().cy;
			pvi->bmiHeader.biSizeImage = m_param.GetSize().cx * m_param.GetSize().cy * 4;
			pMediaType->subtype = MEDIASUBTYPE_RGB32;
			break;
		}
		default:
			return S_FALSE;
		}
		pMediaType->bFixedSizeSamples = TRUE;
		pMediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
		return NOERROR;
	}
}