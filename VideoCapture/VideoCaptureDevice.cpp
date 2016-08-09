#include "stdafx.h"
#include <algorithm>
#include "Common/TinyString.h"
#include "VideoCaptureDevice.h"

namespace Media
{
	GUID MediaSubTypeI420 =
	{
		0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }
	};
	GUID MediaSubTypeHDYC =
	{
		0x43594448, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
	};
	ScopedMediaType::ScopedMediaType()
		:m_mediaType(NULL)
	{

	}
	ScopedMediaType::~ScopedMediaType()
	{
		Release();
	}
	AM_MEDIA_TYPE* ScopedMediaType::operator->()
	{
		return m_mediaType;
	}
	AM_MEDIA_TYPE* ScopedMediaType::Ptr()
	{
		return m_mediaType;
	}
	void ScopedMediaType::Release()
	{
		if (m_mediaType)
		{
			if (m_mediaType->cbFormat != NULL)
			{
				CoTaskMemFree(m_mediaType->pbFormat);
				m_mediaType->cbFormat = 0;
				m_mediaType->pbFormat = NULL;
			}
			if (m_mediaType->pUnk != NULL)
			{
				m_mediaType->pUnk->Release();
				m_mediaType->pUnk = NULL;
			}
			m_mediaType = NULL;
		}
	}
	AM_MEDIA_TYPE** ScopedMediaType::Receive()
	{
		return &m_mediaType;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoCaptureDevice::Name::Name(const string& name, const string& id)
		:m_name(name),
		m_id(id)
	{

	}
	VideoCaptureDevice::Name::~Name()
	{

	}
	const string& VideoCaptureDevice::Name::name() const
	{
		return m_name;
	}
	const string& VideoCaptureDevice::Name::id() const
	{
		return m_id;
	}
	VideoCaptureDevice::VideoCaptureDevice()
		:m_state(Idle)
	{

	}
	VideoCaptureDevice::~VideoCaptureDevice()
	{
		if (m_mediaControl)
		{
			m_mediaControl->Stop();
		}
	}
	BOOL VideoCaptureDevice::Initialize(const Name& name)
	{
		m_currentName = name;
		if (!GetDeviceFilter(m_currentName, &m_captureFilter))
			return FALSE;
		m_capturePin = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_capturePin)
			return FALSE;
		HRESULT hRes = m_graphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_graphBuilder->QueryInterface(&m_mediaControl);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_graphBuilder->AddFilter(m_captureFilter, NULL);
		if (FAILED(hRes))
			return FALSE;
		return CreateCapabilityMap();
	}
	BOOL VideoCaptureDevice::CreateCapabilityMap()
	{
		ASSERT(m_captureFilter);
		ASSERT(m_capturePin);
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_capturePin->QueryInterface(&streamConfig);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IAMVideoControl> videoControl;
		hRes = m_captureFilter->QueryInterface(&videoControl);
		if (FAILED(hRes))
			return FALSE;
		INT count = 0;
		INT size = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&count, &size);
		if (FAILED(hRes))
			return FALSE;
		TinyScopedArray<BYTE> caps(new BYTE[size]);
		for (INT i = 0; i < count; ++i)
		{
			ScopedMediaType mediaType;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), caps.Ptr());
			if (hRes != S_OK)
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video && mediaType->formattype == FORMAT_VideoInfo)
			{
				VideoCaptureCapability capability(i);
				capability.Parameter.SetFormat(TranslateMediaSubtypeToPixelFormat(mediaType->subtype));
				if (capability.Parameter.GetFormat() == PIXEL_FORMAT_UNKNOWN)
					continue;
				VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				capability.Parameter.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);
				REFERENCE_TIME avgTimePerFrame = h->AvgTimePerFrame;
				if (videoControl)
				{
					LONGLONG* rate = NULL;
					LONG listsize = 0;
					SIZE size = capability.Parameter.GetSize();
					hRes = videoControl->GetFrameRateList(m_capturePin, i, size, &listsize, &rate);
					if (hRes == S_OK && listsize > 0 && rate)
					{
						avgTimePerFrame = *std::min_element(rate, rate + listsize);
						CoTaskMemFree(rate);
					}
				}
				capability.Parameter.SetRate((avgTimePerFrame > 0) ? (10000000 / static_cast<FLOAT>(avgTimePerFrame)) : 0.0);
				capability.RateNumerator = capability.Parameter.GetRate();
				capability.RateDenominator = 1;
				m_capabilitys.push_back(capability);
			}
		}
		return !m_capabilitys.empty();
	}
	BOOL VideoCaptureDevice::GetDevices(vector<Name>& names)
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
	BOOL VideoCaptureDevice::GetDeviceFilter(const Name& name, IBaseFilter** ps)
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
	BOOL VideoCaptureDevice::GetPinCategory(IPin* pPin, REFGUID category)
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
	TinyComPtr<IPin> VideoCaptureDevice::GetPin(IBaseFilter* pFilter, PIN_DIRECTION dest, REFGUID category)
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
				if (category == GUID_NULL ||
					GetPinCategory(pin, category))
				{
					return pin;
				}
			}
			pin.Release();
		}
		return pin;
	}
	VideoPixelFormat VideoCaptureDevice::TranslateMediaSubtypeToPixelFormat(const GUID& subType)
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
			{ MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY },
			{ MEDIASUBTYPE_ARGB32, PIXEL_FORMAT_ARGB },
			{ MediaSubTypeHDYC, PIXEL_FORMAT_UYVY },
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
	BOOL VideoCaptureDevice::GetDeviceParams(const VideoCaptureDevice::Name& device, vector<VideoCaptureParam>& params)
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
