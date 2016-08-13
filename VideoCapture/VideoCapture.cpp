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

	void VideoCapture::OnFrameReceive(const BYTE* data, INT size, LPVOID lpData)
	{

	}

	VideoCapture::VideoCapture()
	{

	}
	VideoCapture::~VideoCapture()
	{
		Uninitialize();
	}
	BOOL VideoCapture::Initialize(const Name& name)
	{
		if (!GetDeviceFilter(name, &m_captureFilter))
			return FALSE;

		m_captureConnector = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_captureConnector)
			return FALSE;

		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
			return FALSE;

		hRes = m_builder->QueryInterface(&m_control);
		if (FAILED(hRes))
			return FALSE;

		hRes = CoCreateInstance(CLSID_MjpegDec, 0, CLSCTX_ALL, IID_IBaseFilter, (void**)&m_mjpgFilter);
		if (FAILED(hRes))
			return FALSE;

		m_mjpgConnector1 = GetPin(m_mjpgFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_mjpgConnector1)
			return FALSE;
		m_mjpgConnector2 = GetPin(m_mjpgFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_mjpgConnector2)
			return FALSE;


		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_builder->AddFilter(m_mjpgFilter, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_sinkFilter = new SinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkConnector = m_sinkFilter->GetPin(0);
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
			m_builder->RemoveFilter(m_captureFilter);
		}
		if (m_captureConnector)
		{
			m_captureConnector.Release();
		}
		if (m_sinkConnector)
		{
			m_sinkConnector.Release();
		}
		if (m_captureFilter)
		{
			m_captureFilter.Release();
		}
		if (m_control)
		{
			m_control.Release();
		}
		if (m_builder)
		{
			m_builder.Release();
		}
		m_sinkFilter = NULL;
	}

	//HRESULT EnumMediaTypes(IPin *pin, TinyArray<AM_MEDIA_TYPE*>&types)
	//{
	//	types.RemoveAll();
	//	if (!pin)
	//		return E_POINTER;

	//	IEnumMediaTypes	*emt;
	//	HRESULT			hr;
	//	AM_MEDIA_TYPE	*pmt;
	//	ULONG			f;

	//	hr = pin->EnumMediaTypes(&emt);
	//	if (FAILED(hr)) return hr;

	//	emt->Reset();
	//	while (emt->Next(1, &pmt, &f) == NOERROR)
	//	{
	//		types.Add(pmt);
	//	}
	//	emt->Release();
	//	return NOERROR;
	//}

	//BOOL GetMediaType(const VideoCaptureParam& param, AM_MEDIA_TYPE* mediaType)
	//{
	//	if (mediaType->cbFormat < sizeof(VIDEOINFOHEADER))
	//		return S_FALSE;
	//	if (!mediaType->pbFormat)
	//	{
	//		mediaType->pbFormat = (BYTE *)CoTaskMemAlloc(sizeof(VIDEOINFOHEADER));
	//	}
	//	VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(CoTaskMemAlloc(sizeof(VIDEOINFOHEADER)));
	//	ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
	//	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//	pvi->bmiHeader.biPlanes = 1;
	//	pvi->bmiHeader.biClrImportant = 0;
	//	pvi->bmiHeader.biClrUsed = 0;
	//	if (param.GetRate() > 0)
	//	{
	//		pvi->AvgTimePerFrame = static_cast<FLOAT>(10000000 / param.GetRate());
	//	}
	//	mediaType->majortype = MEDIATYPE_Video;
	//	mediaType->formattype = FORMAT_VideoInfo;
	//	mediaType->bTemporalCompression = FALSE;
	//	pvi->bmiHeader.biCompression = BI_RGB;
	//	pvi->bmiHeader.biBitCount = 24;
	//	pvi->bmiHeader.biWidth = param.GetSize().cx;
	//	pvi->bmiHeader.biHeight = param.GetSize().cy;
	//	pvi->bmiHeader.biSizeImage = (param.GetSize().cx * param.GetSize().cy) * 3;
	//	mediaType->subtype = MEDIASUBTYPE_RGB24;
	//	/*switch (param.GetFormat())
	//	{
	//	case PIXEL_FORMAT_RGB24:
	//	pvi->bmiHeader.biCompression = BI_RGB;
	//	pvi->bmiHeader.biBitCount = 24;
	//	pvi->bmiHeader.biWidth = param.GetSize().cx;
	//	pvi->bmiHeader.biHeight = param.GetSize().cy;
	//	pvi->bmiHeader.biSizeImage = (param.GetSize().cx * param.GetSize().cy) * 3;
	//	mediaType->subtype = MEDIASUBTYPE_RGB24;
	//	break;
	//	case PIXEL_FORMAT_MJPEG:
	//	pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
	//	pvi->bmiHeader.biBitCount = 12;
	//	pvi->bmiHeader.biWidth = param.GetSize().cx;
	//	pvi->bmiHeader.biHeight = param.GetSize().cy;
	//	pvi->bmiHeader.biSizeImage = (param.GetSize().cx * param.GetSize().cy) * 3 / 2;
	//	mediaType->subtype = MEDIASUBTYPE_MJPG;
	//	break;
	//	case PIXEL_FORMAT_I420:
	//	pvi->bmiHeader.biCompression = MAKEFOURCC('I', '4', '2', '0');
	//	pvi->bmiHeader.biBitCount = 12;
	//	pvi->bmiHeader.biWidth = param.GetSize().cx;
	//	pvi->bmiHeader.biHeight = param.GetSize().cy;
	//	pvi->bmiHeader.biSizeImage = (param.GetSize().cx * param.GetSize().cy) * 3 / 2;
	//	mediaType->subtype = MediaSubTypeI420;
	//	break;
	//	case PIXEL_FORMAT_YUY2:
	//	pvi->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
	//	pvi->bmiHeader.biBitCount = 16;
	//	pvi->bmiHeader.biWidth = param.GetSize().cx;
	//	pvi->bmiHeader.biHeight = param.GetSize().cy;
	//	pvi->bmiHeader.biSizeImage = (param.GetSize().cx * param.GetSize().cy) * 2;
	//	mediaType->subtype = MEDIASUBTYPE_YUY2;
	//	break;
	//	default:
	//	return S_FALSE;
	//	}*/
	//	mediaType->bFixedSizeSamples = TRUE;
	//	mediaType->lSampleSize = pvi->bmiHeader.biSizeImage;
	//}

	BOOL VideoCapture::Allocate(const VideoCaptureParam& param)
	{
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_captureConnector->QueryInterface(&streamConfig);
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
					m_sinkFilter->SetParam(param);
					hRes = m_builder->ConnectDirect(m_captureConnector, m_sinkConnector, NULL);
					if (hRes != S_OK)
						return FALSE;
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
			m_builder->Disconnect(m_captureConnector);
			m_builder->Disconnect(m_sinkConnector);
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