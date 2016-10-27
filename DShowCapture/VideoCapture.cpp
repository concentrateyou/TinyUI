#include "stdafx.h"
#include "VideoCapture.h"
#include <ks.h>
#include <ksmedia.h>

namespace DShow
{
	//////////////////////////////////////////////////////////////////////////
	VideoCapture::Name::Name()
	{

	}
	VideoCapture::Name::Name(string&& name, string&& id)
		:m_name(std::move(name)),
		m_id(std::move(id))
	{

	}
	VideoCapture::Name::Name(const string& name, const string& id)
		: m_name(name),
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
	void VideoCapture::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, size, ts, lpParameter);
		}
		else
		{
			if (m_size != size)
			{
				m_bits.Reset(new BYTE[size]);
				m_queue.Initialize(ROUNDUP_POW_2(size * 3));
				m_size = size;
			}
			m_queue.WriteBytes(bits, size);
		}
	}
	VideoCapture::VideoCapture()
		:m_size(0)
	{

	}
	VideoCapture::~VideoCapture()
	{
		Uninitialize();
	}
	BOOL VideoCapture::Initialize(const Name& name)
	{
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
		m_sinkFilter = new VideoSinkFilter(this, &m_lock);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		hRes = m_builder->AddFilter(m_sinkFilter, FILTER_NAME);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL VideoCapture::Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>& callback)
	{
		if (Initialize(name))
		{
			m_callback = callback;
			return TRUE;
		}
		return FALSE;
	}
	void VideoCapture::Uninitialize()
	{
		DeAllocate();
		if (m_control)
			m_control->Pause();
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
					if (h->AvgTimePerFrame > caps.MaxFrameInterval || h->AvgTimePerFrame < caps.MinFrameInterval)
						return FALSE;
					hRes = streamConfig->SetFormat(mediaType.Ptr());
					if (hRes != S_OK)
						return FALSE;
					//1.尝试RGB24硬件是否支持
					VideoCaptureParam ps = param;
					ps.SetFormat(PIXEL_FORMAT_RGB24);
					m_sinkFilter->SetCaptureParam(ps);
					hRes = m_builder->Connect(m_captureO, m_sinkI);
					if (hRes == S_OK)
						return TRUE;
					//2. 如果失败再尝试微软自带的Dec Filter
					switch (param.GetFormat())
					{
					case PIXEL_FORMAT_MJPEG:
					{
						hRes = m_mjpgFilter.CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC);
						if (hRes != S_OK)
							return FALSE;
						hRes = m_builder->AddFilter(m_mjpgFilter, NULL);
						if (hRes != S_OK)
							return FALSE;
						m_mjpgO = GetPin(m_mjpgFilter, PINDIR_OUTPUT, GUID_NULL);
						if (!m_mjpgO)
							return FALSE;
						m_mjpgI = GetPin(m_mjpgFilter, PINDIR_INPUT, GUID_NULL);
						if (!m_mjpgI)
							return FALSE;
						hRes = m_builder->ConnectDirect(m_captureO, m_mjpgI, mediaType.Ptr());
						if (hRes != S_OK)
							return FALSE;
						//判断MjpegDec输出是否支持RGB24
						ScopedMediaType type;
						if (!VideoCapture::GetMediaType(m_mjpgO, MEDIASUBTYPE_RGB24, type.Receive()))
							return FALSE;
						ps.SetFormat(PIXEL_FORMAT_RGB24);
						m_sinkFilter->SetCaptureParam(ps);
						hRes = m_builder->ConnectDirect(m_mjpgO, m_sinkI, NULL);
						if (hRes == S_OK)
							return TRUE;
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
						hRes = m_builder->ConnectDirect(m_captureO, m_avI, mediaType.Ptr());
						if (hRes != S_OK)
							return FALSE;
						//判断AVIDec输出是否支持RGB24
						ScopedMediaType type;
						if (!VideoCapture::GetMediaType(m_avO, MEDIASUBTYPE_RGB24, type.Receive()))
							return FALSE;
						ps.SetFormat(PIXEL_FORMAT_RGB24);
						m_sinkFilter->SetCaptureParam(ps);
						hRes = m_builder->ConnectDirect(m_avO, m_sinkI, NULL);
						if (hRes == S_OK)
							return TRUE;
					}
					break;
					}
				}
			}
		}
		return FALSE;
	}
	void VideoCapture::DeAllocate()
	{
		if (m_builder)
		{
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_sinkI);
			m_builder->Disconnect(m_mjpgI);
			m_builder->Disconnect(m_mjpgO);
			m_builder->Disconnect(m_avI);
			m_builder->Disconnect(m_avO);
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
	BOOL VideoCapture::GetState(FILTER_STATE& state)
	{
		if (!m_control)
			return FALSE;
		return m_control->GetState(0, (OAFilterState*)&state) == S_OK;
	}
	BYTE* VideoCapture::GetPointer()
	{
		m_queue.ReadBytes(m_bits, m_size);
		return m_bits;
	}
	LONG VideoCapture::GetSize()
	{
		return m_size;
	}
	BOOL VideoCapture::ShowProperty(HWND hWND)
	{
		if (!m_captureFilter)
			return FALSE;
		TinyComPtr<ISpecifyPropertyPages> pages;
		if (FAILED(m_captureFilter->QueryInterface(__uuidof(ISpecifyPropertyPages), (void **)&pages)))
			return FALSE;
		CAUUID cauuid;
		if (FAILED(pages->GetPages(&cauuid)))
			return FALSE;
		if (cauuid.cElems > 0)
		{
			FILTER_INFO filterInfo;
			TinyComPtr<IUnknown> unknow;
			if (SUCCEEDED(m_captureFilter->QueryFilterInfo(&filterInfo)) &&
				SUCCEEDED(m_captureFilter->QueryInterface(IID_IUnknown, (void **)&unknow)))
			{
				OleCreatePropertyFrame(hWND, 0, 0, filterInfo.achName, 1, &unknow, cauuid.cElems, cauuid.pElems, 0, 0, NULL);
			}
			CoTaskMemFree(cauuid.pElems);
			filterInfo.pGraph->Release();
			return TRUE;
		}
		return FALSE;
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
				names.push_back(std::move(Name(std::move(name), std::move(id))));
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
