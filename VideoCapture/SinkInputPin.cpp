#include "stdafx.h"
#include "SinkInputPin.h"

namespace Media
{
	const REFERENCE_TIME SecondsToReferenceTime = 10000000;
	SinkInputPin::SinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:PinBase(pFilter, PINDIR_INPUT, PIN_NAME), m_observer(observer)
	{
	}
	SinkInputPin::~SinkInputPin()
	{

	}
	HRESULT STDMETHODCALLTYPE SinkInputPin::Receive(IMediaSample *pSample)
	{
		ASSERT(pSample);
		const INT size = pSample->GetActualDataLength();
		BYTE* data = NULL;
		if (FAILED(pSample->GetPointer(&data)))
			return S_FALSE;
		m_observer->OnFrameReceive(data, size, NULL);
		return S_OK;
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
		/*m_resulting.SetSize(pvi->bmiHeader.biWidth, abs(pvi->bmiHeader.biHeight));
		if (pvi->AvgTimePerFrame > 0)
		{
		m_resulting.SetRate(static_cast<FLOAT>(SecondsToReferenceTime / pvi->AvgTimePerFrame));
		}
		else
		{
		m_resulting.SetRate(m_requesting.GetRate());
		}
		if (subType == MediaSubTypeI420 &&pvi->bmiHeader.biCompression == MAKEFOURCC('I', '4', '2', '0'))
		{
		m_resulting.SetFormat(PIXEL_FORMAT_I420);
		return S_OK;
		}
		if (subType == MEDIASUBTYPE_YUY2 &&pvi->bmiHeader.biCompression == MAKEFOURCC('Y', 'U', 'Y', '2'))
		{
		m_resulting.SetFormat(PIXEL_FORMAT_YUY2);
		return S_OK;
		}
		if (subType == MEDIASUBTYPE_RGB24 && pvi->bmiHeader.biCompression == BI_RGB)
		{
		m_resulting.SetFormat(PIXEL_FORMAT_RGB24);
		return S_OK;
		}*/
		return S_FALSE;
	}
	HRESULT SinkInputPin::GetMediaType(INT index, AM_MEDIA_TYPE* mediaType)
	{
		if (mediaType->cbFormat < sizeof(VIDEOINFOHEADER))
			return S_FALSE;
		VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biClrImportant = 0;
		pvi->bmiHeader.biClrUsed = 0;
		if (m_requesting.GetRate() > 0)
		{
			pvi->AvgTimePerFrame = static_cast<REFERENCE_TIME>(SecondsToReferenceTime / m_requesting.GetRate());
		}
		mediaType->majortype = MEDIATYPE_Video;
		mediaType->formattype = FORMAT_VideoInfo;
		mediaType->bTemporalCompression = FALSE;
		switch (index)
		{
		case 0:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
			pvi->bmiHeader.biBitCount = 12;
			pvi->bmiHeader.biWidth = m_requesting.GetSize().cx;
			pvi->bmiHeader.biHeight = m_requesting.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_requesting.GetSize().cx *  m_requesting.GetSize().cy) * 3 / 2;
			mediaType->subtype = MediaSubTypeI420;
			break;
		}
		case 1:
		{
			pvi->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
			pvi->bmiHeader.biBitCount = 16;
			pvi->bmiHeader.biWidth = m_requesting.GetSize().cx;
			pvi->bmiHeader.biHeight = m_requesting.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_requesting.GetSize().cx *  m_requesting.GetSize().cy) * 2;
			mediaType->subtype = MEDIASUBTYPE_YUY2;
			break;
		}
		case 2:
		{
			pvi->bmiHeader.biCompression = BI_RGB;
			pvi->bmiHeader.biBitCount = 24;
			pvi->bmiHeader.biWidth = m_requesting.GetSize().cx;
			pvi->bmiHeader.biHeight = m_requesting.GetSize().cy;
			pvi->bmiHeader.biSizeImage = (m_requesting.GetSize().cx *  m_requesting.GetSize().cy) * 3;
			mediaType->subtype = MEDIASUBTYPE_RGB24;
			break;
		}
		default:
			return S_FALSE;
		}
		mediaType->bFixedSizeSamples = TRUE;
		mediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
		return S_OK;
	}
	const VideoCaptureParam& SinkInputPin::GetResultingParam()
	{
		return m_resulting;
	}
}