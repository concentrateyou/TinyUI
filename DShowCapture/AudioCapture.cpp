#include "stdafx.h"
#include "AudioCapture.h"

namespace Media
{
	AudioCapture::AudioCapture()
	{
	}

	AudioCapture::~AudioCapture()
	{
	}

	void AudioCapture::OnFrameReceive(const BYTE* data, INT size, LPVOID lpParameter)
	{

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
		return TRUE;
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
				names.push_back(Name(name, id));
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
}
