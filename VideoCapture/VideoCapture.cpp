#include "stdafx.h"
#include "VideoCapture.h"
#include <ks.h>
#include <ksmedia.h>

namespace Media
{
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
		m_sinkFilter = new SinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkConnector = m_sinkFilter->GetPin(0);
		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (FAILED(hRes))
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
			m_control->Stop();
			m_control.Release();
		}
		if (m_builder)
		{
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_captureFilter);
			m_builder.Release();
		}
	}
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
		TinyScopedArray<BYTE> caps(new BYTE[size]);
		for (INT i = 0; i < count; ++i)
		{
			ScopedMediaType mediaType;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), caps.Ptr());
			if (hRes != S_OK)
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video && mediaType->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				if (param.GetFormat() == TranslateMediaSubtypeToPixelFormat(mediaType->subtype) && param.GetSize() == TinySize(h->bmiHeader.biWidth, h->bmiHeader.biHeight))
				{
					SetAntiFlickerInCaptureFilter();
					m_sinkFilter->SetRequestedParam(param);
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
