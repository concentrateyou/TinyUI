#include "stdafx.h"
#include "VideoCapture.h"
#include <ks.h>
#include <ksmedia.h>

namespace Media
{
	//////////////////////////////////////////////////////////////////////////
	VideoCapture::Name::Name()
	{

	}
	VideoCapture::Name::Name(const string& name, const string& id)
		:m_name(name),
		m_id(id)
	{

	}
	VideoCapture::Name::~Name()
	{

	}
	const string& VideoCapture::Name::name() const
	{
		return m_name;
	}
	const string& VideoCapture::Name::id() const
	{
		return m_id;
	}

	void SaveBitmap(BITMAPINFOHEADER bi, INT size, const BYTE* pdata)
	{
		BITMAPFILEHEADER  bmfHeader = { 0 };
		DWORD dwSizeofDIB = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
		bmfHeader.bfSize = dwSizeofDIB;
		bmfHeader.bfType = 0x4D42;
		HANDLE hFile = CreateFile("D:\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwBytesWritten = 0;
		WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
		WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
		WriteFile(hFile, (LPSTR)pdata, size, &dwBytesWritten, NULL);
		CloseHandle(hFile);
	}

	void VideoCapture::OnFrameReceive(const BYTE* data, INT size, LPVOID lpData)
	{
		AM_MEDIA_TYPE* pMediaType = static_cast<AM_MEDIA_TYPE*>(lpData);
		if (pMediaType)
		{
			RECT rectangle = { 0 };
			GetWindowRect(m_hWND, &rectangle);
			VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
			TinyUI::TinyWindowDC wdc(m_hWND);
			BITMAPINFO bi = { 0 };
			bi.bmiHeader = h->bmiHeader;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(wdc, &bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			memcpy(pvBits, data, size);
			TinyUI::TinyMemDC mdc(wdc, hBitmap);
			::BitBlt(wdc, 0, 0, TO_CX(rectangle), TO_CY(rectangle), mdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
		}
	}

	VideoCapture::VideoCapture()
	{

	}
	VideoCapture::~VideoCapture()
	{
		Uninitialize();
	}
	BOOL VideoCapture::Initialize(const Name& name, HWND hWND)
	{
		m_hWND = hWND;
		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (FAILED(hRes))
			return FALSE;
		if (!GetDeviceFilter(name, &m_captureFilter))
			return FALSE;
		m_captureO = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_captureO)
			return FALSE;
		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_sinkFilter = new SinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		hRes = m_builder->AddFilter(m_sinkFilter, NULL);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void VideoCapture::Uninitialize()
	{
		DeAllocate();
		if (m_control)
		{
			m_control->Pause();
		}
		if (m_builder)
		{
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_mjpgFilter);
			m_builder->RemoveFilter(m_avFilter);
			m_builder->RemoveFilter(m_captureFilter);
		}
		m_captureO.Release();
		m_mjpgO.Release();
		m_mjpgI.Release();
		m_avO.Release();
		m_avI.Release();
		m_sinkI.Release();
		m_captureFilter.Release();
		m_avFilter.Release();
		m_mjpgFilter.Release();
		m_control.Release();
		m_builder.Release();
		m_sinkFilter = NULL;
	}

	BOOL VideoCapture::Allocate(const VideoCaptureParam& param)
	{
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_captureO->QueryInterface(&streamConfig);
		if (FAILED(hRes))
			return FALSE;
		INT count = 0;
		INT size = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&count, &size);
		if (FAILED(hRes))
			return FALSE;
		for (INT i = 0; i < count; ++i)
		{
			ScopedMediaType mediaType;
			VIDEO_STREAM_CONFIG_CAPS caps;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), (BYTE*)&caps);
			if (hRes != S_OK)
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video && mediaType->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				if (param.GetFormat() == TranslateMediaSubtypeToPixelFormat(mediaType->subtype) &&
					param.GetSize() == TinySize(h->bmiHeader.biWidth, h->bmiHeader.biHeight))
				{
					SetAntiFlickerInCaptureFilter();
					switch (param.GetFormat())
					{
					case PIXEL_FORMAT_MJPEG:
					{
						hRes = m_mjpgFilter.CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC);
						if (hRes != S_OK)
							return FALSE;
						hRes = m_builder->AddFilter(m_mjpgFilter, NULL);
						if (FAILED(hRes))
							return FALSE;
						m_mjpgO = GetPin(m_mjpgFilter, PINDIR_OUTPUT, GUID_NULL);
						if (!m_mjpgO)
							return FALSE;
						m_mjpgI = GetPin(m_mjpgFilter, PINDIR_INPUT, GUID_NULL);
						if (!m_mjpgI)
							return FALSE;
						h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
						if (h->bmiHeader.biWidth == param.GetSize().cx &&
							h->bmiHeader.biHeight == param.GetSize().cy)
						{
							hRes = m_builder->ConnectDirect(m_captureO, m_mjpgI, mediaType.Ptr());
							if (hRes != S_OK)
								return FALSE;
							ScopedMediaType type;
							if (!VideoCapture::GetMediaType(m_mjpgO, MEDIASUBTYPE_RGB24, type.Receive()))
								return FALSE;
							m_sinkFilter->SetMediaType(type.Ptr());
							hRes = m_builder->ConnectDirect(m_mjpgO, m_sinkI, NULL);
							if (hRes != S_OK)
								return FALSE;
						}
					}
					break;
					case PIXEL_FORMAT_UYVY:
					case PIXEL_FORMAT_YUY2:
					case PIXEL_FORMAT_YV12:
					case PIXEL_FORMAT_I420:
					{
						hRes = m_avFilter.CoCreateInstance(CLSID_AVIDec, NULL, CLSCTX_INPROC);
						if (hRes != S_OK)
							return FALSE;
						hRes = m_builder->AddFilter(m_avFilter, NULL);
						if (FAILED(hRes))
							return FALSE;
						m_avO = GetPin(m_avFilter, PINDIR_OUTPUT, GUID_NULL);
						if (!m_avO)
							return FALSE;
						m_avI = GetPin(m_avFilter, PINDIR_INPUT, GUID_NULL);
						if (!m_avI)
							return FALSE;
						h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
						if (h->bmiHeader.biWidth == param.GetSize().cx &&
							h->bmiHeader.biHeight == param.GetSize().cy)
						{
							hRes = m_builder->ConnectDirect(m_captureO, m_avI, mediaType.Ptr());
							if (hRes != S_OK)
								return FALSE;
							ScopedMediaType type;
							if (!VideoCapture::GetMediaType(m_avO, MEDIASUBTYPE_RGB24, type.Receive()))
								return FALSE;
							m_sinkFilter->SetMediaType(type.Ptr());
							hRes = m_builder->ConnectDirect(m_avO, m_sinkI, NULL);
							if (hRes != S_OK)
								return FALSE;
						}
					}
					break;
					default:
					{
						m_sinkFilter->SetMediaType(mediaType.Ptr());
						hRes = m_builder->ConnectDirect(m_captureO, m_sinkI, NULL);
						if (hRes != S_OK)
							return FALSE;
					}
					break;
					}
					hRes = m_control->Pause();
					if (hRes != S_OK)
						return FALSE;
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	void VideoCapture::DeAllocate()
	{
		if (m_builder)
		{
			if (m_captureO)
			{
				m_builder->Disconnect(m_captureO);
			}
			if (m_sinkI)
			{
				m_builder->Disconnect(m_captureO);
			}
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_sinkI);
		}
	}
	BOOL VideoCapture::Start()
	{
		if (!m_control)
			return FALSE;
		return m_control->Run() == S_OK;
	}
	BOOL VideoCapture::Stop()
	{
		if (!m_control)
			return FALSE;
		return m_control->Stop() == S_OK;
	}
	BOOL VideoCapture::Pause()
	{
		if (!m_control)
			return FALSE;
		return m_control->Pause() == S_OK;
	}
	FILTER_STATE VideoCapture::GetState() const
	{
		return State_Stopped;
	}
	BOOL VideoCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (FAILED(hRes))
			return FALSE;
		names.clear();
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (FAILED(hRes))
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"Description", &variant, 0);
			if (FAILED(hRes))
			{
				hRes = propertyBag->Read(L"FriendlyName", &variant, 0);
			}
			if (SUCCEEDED(hRes) && variant->vt == VT_BSTR)
			{
				const WCHAR* str = V_BSTR(&variant);
				string id;
				string name(WStringToString(str));
				variant.Reset();
				hRes = propertyBag->Read(L"DevicePath", &variant, 0);
				if (FAILED(hRes) || variant->vt != VT_BSTR)
				{
					id = name;
				}
				else
				{
					id = WStringToString(V_BSTR(&variant));
				}
				names.push_back(Name(name, id));
			}
			moniker.Release();
		}
		return TRUE;
	}
	BOOL VideoCapture::GetDeviceFilter(const Name& name, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> dev;
		HRESULT hRes = dev.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		DWORD fetched = 0;
		while (enumMoniker->Next(1, &moniker, &fetched) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (FAILED(hRes))
			{
				moniker.Release();
				continue;
			}
			static const WCHAR* PropertyNames[] =
			{
				L"DevicePath",
				L"Description",
				L"FriendlyName"
			};
			ScopedVariant variant;
			for (INT i = 0; i < ARRAYSIZE(PropertyNames) && variant->vt != VT_BSTR; ++i)
			{
				propertyBag->Read(PropertyNames[i], &variant, 0);
			}
			if (variant->vt == VT_BSTR)
			{
				string device = WStringToString(V_BSTR(variant));
				if (device.compare(name.id()) == 0)
				{
					IBaseFilter* captureFilter = NULL;
					hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&captureFilter);
					if (SUCCEEDED(hRes) && captureFilter)
					{
						*ps = captureFilter;
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}
	BOOL VideoCapture::GetMediaType(IPin* pPin, REFGUID subtype, AM_MEDIA_TYPE** ppType)
	{
		TinyComPtr<IEnumMediaTypes> emt;
		if (FAILED(pPin->EnumMediaTypes(&emt)))
			return FALSE;
		emt->Reset();
		ULONG cFetched;
		AM_MEDIA_TYPE* pMediaType = NULL;
		while (emt->Next(1, &pMediaType, &cFetched) == NOERROR)
		{
			if (pMediaType->majortype == MEDIATYPE_Video &&
				pMediaType->formattype == FORMAT_VideoInfo &&
				pMediaType->subtype == subtype)
			{
				if (*ppType != NULL)
				{
					CopyMediaType(*ppType, pMediaType);
				}
				else
				{
					*ppType = pMediaType;
				}
				return TRUE;
			}
			DeleteMediaType(pMediaType);
		}
		return FALSE;
	}
	BOOL VideoCapture::GetPinCategory(IPin* pPin, REFGUID category)
	{
		ASSERT(pPin);
		BOOL bFlag = FALSE;
		TinyComPtr<IKsPropertySet> ksProperty;
		HRESULT hRes = pPin->QueryInterface(&ksProperty);
		if (SUCCEEDED(hRes))
		{
			GUID category1;
			DWORD dwValue;
			hRes = ksProperty->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, &category1, sizeof(category1), &dwValue);
			if (SUCCEEDED(hRes) && (dwValue == sizeof(category1)))
			{
				bFlag = (category1 == category);
			}
		}
		return bFlag;
	}
	TinyComPtr<IPin> VideoCapture::GetPin(IBaseFilter* pFilter, PIN_DIRECTION dest, REFGUID category)
	{
		ASSERT(pFilter);
		TinyComPtr<IPin> pin;
		TinyComPtr<IEnumPins> enumPins;
		if (FAILED(pFilter->EnumPins(&enumPins)) || !enumPins)
		{
			return pin;
		}
		enumPins->Reset();
		while (enumPins->Next(1, &pin, NULL) == S_OK)
		{
			PIN_DIRECTION src = static_cast<PIN_DIRECTION>(-1);
			if (SUCCEEDED(pin->QueryDirection(&src)) && dest == src)
			{
				if (category == GUID_NULL || GetPinCategory(pin, category))
				{
					return pin;
				}
			}
			pin.Release();
		}
		return pin;
	}
	VideoPixelFormat VideoCapture::TranslateMediaSubtypeToPixelFormat(const GUID& subType)
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
			{ MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2 },
			{ MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG },
			{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY }
		};
		for (size_t i = 0; i < ARRAYSIZE_UNSAFE(formats); ++i)
		{
			if (subType == formats[i].subType)
				return formats[i].format;
		}
		WCHAR str[128];
		StringFromGUID2(subType, str, arraysize(str));
		TRACE("%s", UTF16ToUTF8(str).c_str());
		return PIXEL_FORMAT_UNKNOWN;
	}
	void VideoCapture::SetAntiFlickerInCaptureFilter()
	{
		ASSERT(m_captureFilter);
		TinyComPtr<IKsPropertySet> ksPropset;
		DWORD dwTypeSupport = 0;
		HRESULT hRes;
		if (SUCCEEDED(hRes = m_captureFilter->QueryInterface(&ksPropset)) &&
			SUCCEEDED(hRes = ksPropset->QuerySupported(PROPSETID_VIDCAP_VIDEOPROCAMP, KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, &dwTypeSupport)) &&
			(dwTypeSupport & KSPROPERTY_SUPPORT_SET))
		{
			KSPROPERTY_VIDEOPROCAMP_S data = {};
			data.Property.Set = PROPSETID_VIDCAP_VIDEOPROCAMP;
			data.Property.Id = KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY;
			data.Property.Flags = KSPROPERTY_TYPE_SET;
			data.Value = 2;
			data.Flags = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
			hRes = ksPropset->Set(PROPSETID_VIDCAP_VIDEOPROCAMP, KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, &data, sizeof(data), &data, sizeof(data));
		}
	}
	BOOL VideoCapture::GetDeviceParams(const VideoCapture::Name& device, vector<VideoCaptureParam>& params)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IBaseFilter> captureFilter;
		if (!GetDeviceFilter(device, &captureFilter))
			return FALSE;
		TinyComPtr<IPin> outputPin(GetPin(captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE));
		if (!outputPin)
			return FALSE;
		TinyComPtr<IAMStreamConfig> streamConfig;
		hRes = outputPin->QueryInterface(&streamConfig);
		if (FAILED(hRes))
			return FALSE;
		INT iCount = 0;
		INT iSize = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (FAILED(hRes))
			return FALSE;
		TinyScopedArray<BYTE> caps(new BYTE[iSize]);
		for (INT i = 0; i < iCount; ++i)
		{
			ScopedMediaType mediaType;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), caps.Ptr());
			if (FAILED(hRes))
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video &&mediaType->formattype == FORMAT_VideoInfo)
			{
				VideoCaptureParam param;
				param.SetFormat(TranslateMediaSubtypeToPixelFormat(mediaType->subtype));
				if (param.GetFormat() == PIXEL_FORMAT_UNKNOWN)
					continue;
				VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				param.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);
				param.SetRate((h->AvgTimePerFrame > 0) ? 10000000 / static_cast<FLOAT>(h->AvgTimePerFrame) : 0.0F);
				params.push_back(param);
			}
		}
		return TRUE;
	}
}
