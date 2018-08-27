#include "stdafx.h"
#include "AudioCapture.h"
#include "ScopedMediaType.h"

namespace DShow
{
	AudioCapture::Name::Name()
	{

	}
	AudioCapture::Name::Name(const string& name, const string& id)
		:m_name(name),
		m_id(id)
	{

	}
	AudioCapture::Name::Name(string&& name, string&& id)
		: m_name(std::move(name)),
		m_id(std::move(id))
	{

	}
	AudioCapture::Name::~Name()
	{

	}
	const string& AudioCapture::Name::name() const
	{
		return m_name;
	}
	const string& AudioCapture::Name::id() const
	{
		return m_id;
	}

	AudioCapture::AudioCapture()
	{
	}

	AudioCapture::~AudioCapture()
	{
		Uninitialize();
	}
	BOOL	AudioCapture::IsEmpty() const
	{
		return  (m_builder == NULL ||
			m_sinkFilter == NULL ||
			m_captureO == NULL ||
			m_control == NULL);
	}
	void AudioCapture::OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, size, timestamp, lpParameter);
		}
	}
	void AudioCapture::SetCallback(Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>&& callback)
	{
		m_callback = std::move(callback);
	}
	BOOL AudioCapture::Initialize(const Name& name)
	{
		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (hRes != S_OK)
			return FALSE;
		if (!GetDeviceFilter(name, &m_captureFilter))
			return FALSE;
		hRes = m_captureFilter->QueryInterface(&m_mixer);
		if (hRes != S_OK)
			return FALSE;
		m_captureO = GetPin(m_captureFilter, PINDIR_OUTPUT, PIN_CATEGORY_CAPTURE);
		if (!m_captureO)
			return FALSE;
		hRes = m_builder->AddFilter(m_captureFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_sinkFilter = new AudioSinkFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		hRes = m_builder->AddFilter(m_sinkFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void AudioCapture::Uninitialize()
	{
		Pause();
		Deallocate();
		if (m_builder != NULL)
		{
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_captureFilter);
		}
		m_captureO.Release();
		m_mixer.Release();
		m_captureFilter.Release();
		m_sinkI.Release();
		m_control.Release();
		m_builder.Release();
		m_sinkFilter = NULL;
	}
	BOOL AudioCapture::SetVolume(LONG volume)
	{
		if (m_mixer != NULL)
		{
			DOUBLE dVolume = ((DOUBLE)volume / 100);
			return SUCCEEDED(m_mixer->put_MixLevel(dVolume));
		}
		return FALSE;
	}
	BOOL AudioCapture::GetVolume(LONG& volume)
	{
		if (m_mixer != NULL)
		{
			DOUBLE dVolume = 0.0;
			HRESULT hRes = m_mixer->get_MixLevel(&dVolume);
			volume = static_cast<LONG>((dVolume * 100));
			return SUCCEEDED(hRes);
		}
		return FALSE;
	}
	BOOL AudioCapture::Start()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Run());
		return FALSE;
	}
	BOOL AudioCapture::Stop()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Stop());
		return FALSE;
	}
	BOOL AudioCapture::Pause()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Pause());
		return FALSE;
	}
	BOOL AudioCapture::GetState(FILTER_STATE& state)
	{
		if (!m_control)
			return FALSE;
		return m_control->GetState(0, (OAFilterState*)&state) == S_OK;
	}
	BOOL AudioCapture::Allocate(const AudioCaptureParam& param)
	{
		if (IsEmpty())
			return FALSE;
		TinyComPtr<IAMStreamConfig> streamConfig;
		HRESULT hRes = m_captureO->QueryInterface(&streamConfig);
		if (hRes != S_OK)
			return FALSE;
		INT count = 0;
		INT size = 0;
		hRes = streamConfig->GetNumberOfCapabilities(&count, &size);
		if (hRes != S_OK)
			return FALSE;
		WAVEFORMATEX w = param.RequestFormat.GetFormat();
		for (INT i = 0; i < count; ++i)
		{
			ScopedMediaType mediaType;
			AUDIO_STREAM_CONFIG_CAPS caps;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), (BYTE*)&caps);
			if (hRes != S_OK)
				return FALSE;
			//目前只支持PCM采集
			if (mediaType->majortype == MEDIATYPE_Audio && mediaType->formattype == FORMAT_WaveFormatEx)
			{
				WAVEFORMATEX *pvi = reinterpret_cast<WAVEFORMATEX*>(mediaType->pbFormat);
				if (w.wFormatTag == pvi->wFormatTag &&
					w.nChannels == pvi->nChannels &&
					w.nSamplesPerSec == pvi->nSamplesPerSec &&
					w.wBitsPerSample == pvi->wBitsPerSample)
				{
					TinyComPtr<IAMBufferNegotiation> neg;
					if (SUCCEEDED(m_captureO->QueryInterface(IID_IAMBufferNegotiation, (void**)&neg)))
					{
						ALLOCATOR_PROPERTIES prop = { 0 };
						prop.cbBuffer = 1024 * w.nBlockAlign;
						prop.cBuffers = 4;
						prop.cbAlign = 1;
						hRes = neg->SuggestAllocatorProperties(&prop);
					}
					m_sinkFilter->SetRequestFormat(param.RequestFormat);
					hRes = streamConfig->SetFormat(mediaType.Ptr());
					if (hRes != S_OK)
						return FALSE;
					hRes = m_builder->Connect(m_captureO, m_sinkI);
					if (hRes != S_OK)
						return FALSE;
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	void AudioCapture::Deallocate()
	{
		if (m_builder != NULL)
		{
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_sinkI);
		}
	}
	BOOL AudioCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
			string friendlyName;
			ScopedVariant variant;
			hRes = propertyBag->Read(L"Description", &variant, 0);
			if (hRes != S_OK)
			{
				hRes = propertyBag->Read(L"FriendlyName", &variant, 0);
			}
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
	BOOL AudioCapture::GetDeviceFilter(const Name& name, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> dev;
		HRESULT hRes = dev.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
	BOOL AudioCapture::GetDeviceFormats(const AudioCapture::Name& device, vector<AudioCaptureFormat>& formats)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
			if (mediaType->majortype == MEDIATYPE_Audio &&mediaType->formattype == FORMAT_WaveFormatEx)
			{
				WAVEFORMATEX *pFormat = reinterpret_cast<WAVEFORMATEX*>(mediaType->pbFormat);
				if (!pFormat) continue;
				AudioCaptureFormat s;
				s.SetFormat(*pFormat);
				formats.push_back(s);
			}
		}
		return TRUE;
	}
}
