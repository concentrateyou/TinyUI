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
		:m_size(0)
	{
	}

	AudioCapture::~AudioCapture()
	{
		Uninitialize();
	}

	void AudioCapture::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
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
	BOOL AudioCapture::Initialize(const Name& name)
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
		m_sinkFilter = new AudioSinkFilter(this);
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
	BOOL AudioCapture::Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>& callback)
	{
		if (Initialize(name))
		{
			m_callback = callback;
			return TRUE;
		}
		return FALSE;
	}
	void AudioCapture::Uninitialize()
	{
		DeAllocate();
		if (m_control)
			m_control->Pause();
		if (m_builder)
		{
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_captureFilter);
		}
		m_captureO.Release();
		m_captureFilter.Release();
		m_sinkI.Release();
		m_control.Release();
		m_builder.Release();
		m_sinkFilter = NULL;
	}
	void AudioCapture::SetVolume(INT volume)
	{

	}
	INT AudioCapture::GetVolume() const
	{
		return 0;
	}
	BOOL AudioCapture::Start()
	{
		if (!m_control)
			return FALSE;
		return m_control->Run() == S_OK;
	}
	BOOL AudioCapture::Stop()
	{
		if (!m_control)
			return FALSE;
		return m_control->Stop() == S_OK;
	}
	BOOL AudioCapture::Pause()
	{
		if (!m_control)
			return FALSE;
		return m_control->Pause() == S_OK;
	}
	BYTE* AudioCapture::GetPointer()
	{
		m_queue.ReadBytes(m_bits, m_size);
		return m_bits;
	}
	LONG AudioCapture::GetSize()
	{
		return m_size;
	}
	BOOL AudioCapture::GetState(FILTER_STATE& state)
	{
		if (!m_control)
			return FALSE;
		return m_control->GetState(0, (OAFilterState*)&state) == S_OK;
	}
	BOOL AudioCapture::Allocate(const AudioCaptureParam& param)
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
		WAVEFORMATEX w = param.GetFormat();
		for (INT i = 0; i < count; ++i)
		{
			ScopedMediaType mediaType;
			AUDIO_STREAM_CONFIG_CAPS caps;
			hRes = streamConfig->GetStreamCaps(i, mediaType.Receive(), (BYTE*)&caps);
			if (FAILED(hRes))
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
						prop.cbBuffer = 4096;
						prop.cBuffers = 4;
						prop.cbAlign = 1;
						hRes = neg->SuggestAllocatorProperties(&prop);
					}
					hRes = streamConfig->SetFormat(mediaType.Ptr());
					if (hRes != S_OK)
						return FALSE;
					m_sinkFilter->SetCaptureParam(param);
					hRes = m_builder->Connect(m_captureO, m_sinkI);
					if (hRes != S_OK)
						return FALSE;
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	void AudioCapture::DeAllocate()
	{
		if (m_builder)
		{
			m_builder->Disconnect(m_captureO);
			m_builder->Disconnect(m_sinkI);
		}
	}
	BOOL AudioCapture::GetPinCategory(IPin* pPin, REFGUID category)
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
	TinyComPtr<IPin> AudioCapture::GetPin(IBaseFilter* pFilter, PIN_DIRECTION dest, REFGUID category)
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
	BOOL AudioCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
	BOOL AudioCapture::GetDeviceFilter(const Name& name, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> dev;
		HRESULT hRes = dev.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = dev->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
	BOOL AudioCapture::GetDeviceParams(const AudioCapture::Name& device, vector<AudioCaptureParam>& params)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
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
			if (mediaType->majortype == MEDIATYPE_Audio &&mediaType->formattype == FORMAT_WaveFormatEx)
			{
				WAVEFORMATEX *pFormat = reinterpret_cast<WAVEFORMATEX*>(mediaType->pbFormat);
				if (!pFormat) continue;
				AudioCaptureParam param;
				param.SetFormat(*pFormat);
				params.push_back(param);
			}
		}
		return TRUE;
	}
}
