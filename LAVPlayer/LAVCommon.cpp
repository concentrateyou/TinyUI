#include "stdafx.h"
#include "LAVCommon.h"

namespace LAV
{
	BOOL WINAPI GetFilterByCLSID(const string& clsid, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"CLSID", &variant, 0);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			string value = std::move(WStringToString(V_BSTR(&variant)));
			if (value.compare(clsid) == 0)
			{
				IBaseFilter* baseFilter = NULL;
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&baseFilter);
				if (SUCCEEDED(hRes) && baseFilter)
				{
					*ps = baseFilter;
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	ScopedBSTR::ScopedBSTR(const CHAR* psText)
		:m_bstr(NULL)
	{

	}
	ScopedBSTR::~ScopedBSTR()
	{

	}


	LAVVideoFormat::LAVVideoFormat()
		:m_vpf(PIXEL_FORMAT_UNKNOWN)
	{

	}
	LAVVideoFormat::LAVVideoFormat(const TinySize& size, VideoPixelFormat vpf)
		: m_size(size),
		m_vpf(vpf)
	{

	}
	string LAVVideoFormat::PixelFormatToString(VideoPixelFormat vpf)
	{
		switch (vpf)
		{
		case PIXEL_FORMAT_UNKNOWN:
			return "UNKNOWN";
		case PIXEL_FORMAT_I420:
			return "I420";
		case PIXEL_FORMAT_YUY2:
			return "YUY2";
		case PIXEL_FORMAT_UYVY:
			return "UYUY";
		case PIXEL_FORMAT_RGB24:
			return "RGB24";
		case PIXEL_FORMAT_MJPEG:
			return "MJPEG";
		case PIXEL_FORMAT_YV12:
			return "YV12";
		case PIXEL_FORMAT_NV12:
			return "NV12";
		case PIXEL_FORMAT_MAX:
			break;
		}
		return "";
	}
	BOOL LAVVideoFormat::IsValid() const
	{
		return (m_size.cx < MaxDimension) &&
			(m_size.cy < MaxDimension) &&
			(m_size.cx * m_size.cy >= 0) &&
			(m_size.cx * m_size.cy < MaxCanvas) &&
			(m_vpf >= PIXEL_FORMAT_UNKNOWN) &&
			(m_vpf < PIXEL_FORMAT_MAX);
	}
	void LAVVideoFormat::SetFormat(VideoPixelFormat vpf)
	{
		m_vpf = vpf;
	}
	void LAVVideoFormat::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	void LAVVideoFormat::SetSize(INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
	}
	void LAVVideoFormat::Reset()
	{
		m_size.Empty();
		m_vpf = PIXEL_FORMAT_UNKNOWN;
	}
	const TinySize& LAVVideoFormat::GetSize() const
	{
		return m_size;
	}
	const VideoPixelFormat LAVVideoFormat::GetFormat() const
	{
		return m_vpf;
	}
	const string LAVVideoFormat::ToString() const
	{
		return StringPrintf("%s,%d*%d", PixelFormatToString(m_vpf).c_str(), m_size.cx, m_size.cy);
	}


	LAVAudioFormat::LAVAudioFormat()
	{

	}
	LAVAudioFormat::~LAVAudioFormat()
	{

	}
	WAVEFORMATEX LAVAudioFormat::GetFormat() const
	{
		return m_wft;
	}
	void LAVAudioFormat::SetFormat(const WAVEFORMATEX& wfx)
	{
		m_wft = wfx;
	}
	string LAVAudioFormat::ToString() const
	{
		return StringPrintf("%d,%d,%d,%d,%d", m_wft.nAvgBytesPerSec, m_wft.nBlockAlign, m_wft.nChannels, m_wft.nSamplesPerSec, m_wft.wBitsPerSample);
	}
}
