#include "stdafx.h"
#include "VideoCapture.h"
#include <ks.h>
#include <ksmedia.h>

namespace DShow
{
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
	void VideoCapture::OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, size, timestamp, lpParameter);
		}
	}
	VideoCapture::VideoCapture()
	{

	}
	VideoCapture::~VideoCapture()
	{
		Uninitialize();
	}
	BOOL VideoCapture::IsEmpty() const
	{
		return (m_builder == NULL ||
			m_sinkFilter == NULL ||
			m_captureO == NULL ||
			m_control == NULL);
	}
	void VideoCapture::SetCallback(Callback<void(BYTE*, LONG, INT64, LPVOID)>&& callback)
	{
		m_callback = std::move(callback);
	}
	BOOL VideoCapture::Initialize(const Name& name)
	{
		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (hRes != S_OK)
			return FALSE;
		if (!GetDeviceFilter(name, &m_captureFilter))
			return FALSE;
		m_captureO = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_captureO)
			return FALSE;
		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_sinkFilter = new VideoSinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		hRes = m_builder->AddFilter(m_sinkFilter, FILTER_NAME);
		if (hRes != S_OK)
			return FALSE;
		m_cFormat.Reset();
		return TRUE;
	}
	void VideoCapture::Uninitialize()
	{
		m_cFormat.Reset();
		Pause();
		Deallocate();
		if (m_builder != NULL)
		{
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_mjpgFilter);
			m_builder->RemoveFilter(m_avFilter);
			m_builder->RemoveFilter(m_captureFilter);
		}
		m_captureO.Release();
		m_sinkI.Release();
		m_mjpgI.Release();
		m_mjpgO.Release();
		m_avO.Release();
		m_avI.Release();
		m_mjpgFilter.Release();
		m_avFilter.Release();
		m_captureFilter.Release();
		m_control.Release();
		m_builder.Release();
		m_sinkFilter = NULL;

	}
	BOOL VideoCapture::Allocate(const VideoCaptureParam& cp)
	{
		if (IsEmpty())
			return FALSE;
		m_cFormat.Reset();
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_captureO->QueryInterface(&streamConfig);
		if (hRes != S_OK)
			return FALSE;
		INT count = 0;
		INT size = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&count, &size);
		if (hRes != S_OK)
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
				VideoCaptureFormat request = cp.RequestFormat;
				VIDEOINFOHEADER* vih = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				if (request.GetFormat() == TranslateMediaSubtypeToPixelFormat(mediaType->subtype) &&
					request.GetSize() == TinySize(vih->bmiHeader.biWidth, vih->bmiHeader.biHeight))
				{
					SetAntiFlickerInCaptureFilter();
					if (request.GetRate() > 0.0F)
					{
						vih->AvgTimePerFrame = SecondsToReferenceTime / static_cast<REFERENCE_TIME>(request.GetRate());
					}
					//Ê×ÏÈ³¢ÊÔRGB32
					request.SetFormat(PIXEL_FORMAT_YUY2);
					m_sinkFilter->SetRequestFormat(request);
					hRes = streamConfig->SetFormat(mediaType.Ptr());
					if (hRes != S_OK)
						return FALSE;
					hRes = m_builder->Connect(m_captureO, m_sinkI);
					if (hRes != S_OK)
					{
						switch (request.GetFormat())
						{
						case PIXEL_FORMAT_MJPEG:
						{
							if (!m_mjpgFilter)
							{
								hRes = m_mjpgFilter.CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC);
								if (hRes != S_OK)
									return FALSE;
								m_mjpgO = GetPin(m_mjpgFilter, PINDIR_OUTPUT, GUID_NULL);
								m_mjpgI = GetPin(m_mjpgFilter, PINDIR_INPUT, GUID_NULL);
								hRes = m_builder->AddFilter(m_mjpgFilter, NULL);
								if (hRes != S_OK)
								{
									m_mjpgFilter.Release();
									m_mjpgO.Release();
									m_mjpgI.Release();
									return FALSE;
								}
							}
							if (m_mjpgFilter != NULL)
							{
								hRes = m_builder->ConnectDirect(m_captureO, m_mjpgI, mediaType.Ptr());
								if (hRes != S_OK)
									return FALSE;
								ScopedMediaType type;
								if (!VideoCapture::GetMediaType(m_mjpgO, MEDIASUBTYPE_RGB32, type.Receive()))
									return FALSE;
								request.SetFormat(PIXEL_FORMAT_RGB32);
								m_sinkFilter->SetRequestFormat(request);
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
							if (!m_avFilter)
							{
								hRes = m_avFilter.CoCreateInstance(CLSID_AVIDec, NULL, CLSCTX_INPROC);
								if (hRes != S_OK)
									return FALSE;
								m_avO = GetPin(m_avFilter, PINDIR_OUTPUT, GUID_NULL);
								m_avI = GetPin(m_avFilter, PINDIR_INPUT, GUID_NULL);
								hRes = m_builder->AddFilter(m_avFilter, NULL);
								if (hRes != S_OK)
								{
									m_avFilter.Release();
									m_avO.Release();
									m_avI.Release();
									return FALSE;
								}
							}
							if (m_avFilter != NULL)
							{
								hRes = m_builder->ConnectDirect(m_captureO, m_avI, mediaType.Ptr());
								if (hRes != S_OK)
									return FALSE;
								ScopedMediaType type;
								if (!VideoCapture::GetMediaType(m_avO, MEDIASUBTYPE_RGB32, type.Receive()))
									return FALSE;
								request.SetFormat(PIXEL_FORMAT_RGB32);
								m_sinkFilter->SetRequestFormat(request);
								hRes = m_builder->ConnectDirect(m_avO, m_sinkI, NULL);
								if (hRes != S_OK)
									return FALSE;
							}
						}
						break;
						}
					}
					m_cFormat = m_sinkFilter->GetResponseFormat();
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	void VideoCapture::Deallocate()
	{
		if (m_builder != NULL)
		{
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_mjpgI);
			m_builder->Disconnect(m_mjpgO);
			m_builder->Disconnect(m_avI);
			m_builder->Disconnect(m_avO);
			m_builder->Disconnect(m_sinkI);
		}
	}
	BOOL VideoCapture::Start()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Run());
		return FALSE;
	}
	BOOL VideoCapture::Stop()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Stop());
		return FALSE;
	}
	BOOL VideoCapture::Pause()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Pause());
		return FALSE;
	}
	BOOL VideoCapture::GetState(FILTER_STATE& state)
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->GetState(0, (OAFilterState*)&state));
		return FALSE;
	}
	const VideoCaptureFormat& VideoCapture::GetFormat()
	{
		return m_cFormat;
	}
	BOOL VideoCapture::ShowProperty(HWND hWND)
	{
		if (m_captureFilter != NULL)
		{
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
		}
		return FALSE;
	}
	BOOL VideoCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		names.clear();
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
			hRes = propertyBag->Read(L"Description", &variant, 0);
			if (hRes != S_OK)
			{
				hRes = propertyBag->Read(L"FriendlyName", &variant, 0);
			}
			string friendlyName;
			if (SUCCEEDED(hRes) && variant->vt == VT_BSTR)
			{
				string id;
				friendlyName = std::move(WStringToString(V_BSTR(&variant)));
				variant.Reset();
				hRes = propertyBag->Read(L"DevicePath", &variant, 0);
				if (FAILED(hRes) || variant->vt != VT_BSTR)
				{
					id = friendlyName;
				}
				else
				{
					id = WStringToString(V_BSTR(&variant));
				}
				names.push_back(std::move(Name(std::move(friendlyName), std::move(id))));
			}
			moniker.Release();
		}
		return TRUE;
	}
	BOOL VideoCapture::GetDeviceFilter(const Name& name, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> dev;
		HRESULT hRes = dev.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		DWORD fetched = 0;
		while (enumMoniker->Next(1, &moniker, &fetched) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != S_OK)
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
	BOOL VideoCapture::GetDeviceFormats(const VideoCapture::Name& device, vector<VideoCaptureFormat>& formats)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IBaseFilter> captureFilter;
		if (!GetDeviceFilter(device, &captureFilter))
			return FALSE;
		TinyComPtr<IPin> outputPin(GetPin(captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE));
		if (!outputPin)
			return FALSE;
		TinyComPtr<IAMStreamConfig> streamConfig;
		hRes = outputPin->QueryInterface(&streamConfig);
		if (hRes != S_OK)
			return FALSE;
		INT iCount = 0;
		INT iSize = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (hRes != S_OK)
			return FALSE;
		TinyScopedArray<BYTE> caps(new BYTE[iSize]);
		for (INT i = 0; i < iCount; ++i)
		{
			ScopedMediaType mediaType;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), caps.Ptr());
			if (hRes != S_OK)
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video &&mediaType->formattype == FORMAT_VideoInfo)
			{
				VideoCaptureFormat vcf;
				vcf.SetFormat(TranslateMediaSubtypeToPixelFormat(mediaType->subtype));
				if (vcf.GetFormat() == PIXEL_FORMAT_UNKNOWN)
					continue;
				VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
				vcf.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);
				vcf.SetRate((h->AvgTimePerFrame > 0) ? 10000000 / static_cast<FLOAT>(h->AvgTimePerFrame) : 0.0F);
				formats.push_back(vcf);
			}
		}
		return TRUE;
	}
}
