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
	BYTE clip255(LONG v)
	{
		if (v < 0)
			v = 0;
		else if (v > 255)
			v = 255;
		return (BYTE)v;
	}
	void YUY2_RGB(const BYTE *YUY2buff, BYTE *RGBbuff, DWORD dwSize)
	{
		BYTE *orgRGBbuff = RGBbuff;
		for (DWORD count = 0; count < dwSize; count += 4)
		{
			//Y0 U0 Y1 V0  
			BYTE Y0 = *YUY2buff;
			BYTE U = *(++YUY2buff);
			BYTE Y1 = *(++YUY2buff);
			BYTE V = *(++YUY2buff);
			++YUY2buff;
			LONG Y, C, D, E;
			BYTE R, G, B;

			Y = Y0;
			C = Y - 16;
			D = U - 128;
			E = V - 128;
			R = clip255((298 * C + 409 * E + 128) >> 8);
			G = clip255((298 * C - 100 * D - 208 * E + 128) >> 8);
			B = clip255((298 * C + 516 * D + 128) >> 8);
			*(RGBbuff) = B;
			*(++RGBbuff) = G;
			*(++RGBbuff) = R;
			Y = Y1;
			C = Y - 16;
			D = U - 128;
			E = V - 128;
			R = clip255((298 * C + 409 * E + 128) >> 8);
			G = clip255((298 * C - 100 * D - 208 * E + 128) >> 8);
			B = clip255((298 * C + 516 * D + 128) >> 8);
			*(++RGBbuff) = B;
			*(++RGBbuff) = G;
			*(++RGBbuff) = R;
			++RGBbuff;

		}
	}
	void VideoCapture::OnFrameReceive(const BYTE* data, INT size, LPVOID lpData)
	{
		AM_MEDIA_TYPE* pMediaType = static_cast<AM_MEDIA_TYPE*>(lpData);
		if (pMediaType)
		{
			VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
			int rgb24size = h->bmiHeader.biWidth * h->bmiHeader.biHeight * 3;
			uint8_t* buffer = (uint8_t*)_aligned_malloc(rgb24size, 32);//RGB24
			m_converter->convert(data, buffer);
			//int rgb24size = h->bmiHeader.biWidth * h->bmiHeader.biHeight * 3;
			//uint8_t* buffer = (uint8_t*)_aligned_malloc(rgb24size, 32);//RGB24
			BITMAPINFOHEADER bi = { 0 };
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biBitCount = 24;
			bi.biCompression = BI_RGB;
			bi.biWidth = h->bmiHeader.biWidth;
			bi.biHeight = h->bmiHeader.biHeight;
			bi.biPlanes = 1;
			bi.biSizeImage = rgb24size;
			SaveBitmap(bi, rgb24size, buffer);
			//_aligned_free(buffer);
			//int rgb24size = h->bmiHeader.biWidth * h->bmiHeader.biHeight * 3;
			//uint8_t* buffer = (uint8_t*)_aligned_malloc(rgb24size, 32);//RGB24
			//m_converter->convert(data, buffer);
			//SaveBitmap(h->bmiHeader, rgb24size, buffer);
			/*RECT rectangle = { 0 };
			GetWindowRect(m_hWND, &rectangle);
			TinyUI::TinyWindowDC wdc(m_hWND);
			BITMAPINFO bi = { 0 };
			bi.bmiHeader = h->bmiHeader;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(wdc, &bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			memcpy(pvBits, buffer, rgb24size);
			TinyUI::TinyMemDC mdc(wdc, hBitmap);
			::BitBlt(wdc, 0, 0, TO_CX(rectangle), TO_CY(rectangle), mdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);*/
			//_aligned_free(buffer);
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
		if (hRes != NOERROR)
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (hRes != NOERROR)
			return FALSE;
		if (!GetDeviceFilter(name, &m_captureFilter))
			return FALSE;
		m_captureO = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_captureO)
			return FALSE;
		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (hRes != NOERROR)
			return FALSE;
		m_sinkFilter = new SinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		hRes = m_builder->AddFilter(m_sinkFilter, NULL);
		if (hRes != NOERROR)
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
			m_builder->RemoveFilter(m_captureFilter);
		}
		m_captureO.Release();
		m_sinkI.Release();
		m_captureFilter.Release();
		m_control.Release();
		m_builder.Release();
		m_sinkFilter = NULL;
	}

	BOOL VideoCapture::Allocate(const VideoCaptureParam& param)
	{
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_captureO->QueryInterface(&streamConfig);
		if (hRes != NOERROR)
			return FALSE;
		INT count = 0;
		INT size = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&count, &size);
		if (hRes != NOERROR)
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
					//SetAntiFlickerInCaptureFilter();
					m_sinkFilter->SetMediaType(mediaType.Ptr());
					if (mediaType->subtype == MEDIASUBTYPE_YUY2)
					{
						m_converter.Reset(new BitmapConverter(BitmapConverter::YUV422, h->bmiHeader.biWidth, h->bmiHeader.biHeight, BitmapConverter::RGB24, h->bmiHeader.biWidth, h->bmiHeader.biHeight, true));
					}
					return m_builder->ConnectDirect(m_captureO, m_sinkI, NULL) != S_OK;
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
		if (hRes != NOERROR)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != NOERROR)
			return FALSE;
		names.clear();
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != NOERROR)
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"Description", &variant, 0);
			if (hRes != NOERROR)
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
		if (hRes != NOERROR)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != NOERROR)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		DWORD fetched = 0;
		while (enumMoniker->Next(1, &moniker, &fetched) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != NOERROR)
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
		if (hRes != NOERROR)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != NOERROR)
			return FALSE;
		TinyComPtr<IBaseFilter> captureFilter;
		if (!GetDeviceFilter(device, &captureFilter))
			return FALSE;
		TinyComPtr<IPin> outputPin(GetPin(captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE));
		if (!outputPin)
			return FALSE;
		TinyComPtr<IAMStreamConfig> streamConfig;
		hRes = outputPin->QueryInterface(&streamConfig);
		if (hRes != NOERROR)
			return FALSE;
		INT iCount = 0;
		INT iSize = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (hRes != NOERROR)
			return FALSE;
		TinyScopedArray<BYTE> caps(new BYTE[iSize]);
		for (INT i = 0; i < iCount; ++i)
		{
			ScopedMediaType mediaType;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), caps.Ptr());
			if (hRes != NOERROR)
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
