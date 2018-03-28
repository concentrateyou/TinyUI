#include "stdafx.h"
#include "LAVPlayer.h"

namespace LAV
{
	LAVPlayer::LAVPlayer()
	{
	}

	LAVPlayer::~LAVPlayer()
	{
	}
	BOOL LAVPlayer::Initialize()
	{
		HRESULT hRes = m_builder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->QueryInterface(&m_control);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->QueryInterface(&m_seeking);
		if (hRes != S_OK)
			return FALSE;
		if (!InitializeLAVS())
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::InitializeLAVS()
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
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
			hRes = propertyBag->Read(L"CLSID", &variant, 0);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			string clsid = std::move(WStringToString(V_BSTR(&variant)));
			if (clsid.compare("{B98D13E7-55DB-4385-A33D-09FD1BA26338}") == 0)
			{
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_lavFilter);
				if (hRes != S_OK || !m_lavFilter)
				{
					return FALSE;
				}
			}
			if (clsid.compare("{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}") == 0)
			{
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_audioFilter);
				if (hRes != S_OK || !m_audioFilter)
				{
					return FALSE;
				}
			}
			if (clsid.compare("{EE30215D-164F-4A92-A4EB-9D4C13390F9F}") == 0)
			{
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_videoFilter);
				if (hRes != S_OK || !m_videoFilter)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	BOOL LAVPlayer::GetFilterByCLSID(const string& clsid, IBaseFilter** ps)
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
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
			hRes = propertyBag->Read(L"CLSID", &variant, 0);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			string value = std::move(WStringToString(V_BSTR(&variant)));
			if (value.compare(clsid) == 0)
			{
				IBaseFilter* baseFilter = NULL;
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&baseFilter);
				if (SUCCEEDED(hRes) && baseFilter)
				{
					*ps = baseFilter;
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	BOOL LAVPlayer::Open(LPCSTR pzFile)
	{
		if (!PathFileExists(pzFile))
			return FALSE;
		if (!Initialize())
			return FALSE;
		TinyComPtr<IFileSourceFilter> sourceFilter;
		HRESULT hRes = m_lavFilter->QueryInterface(&sourceFilter);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_lavFilter->QueryInterface(&m_asmstream);
		if (hRes != S_OK)
			return FALSE;
		wstring wsz = StringToWString(pzFile);
		hRes = sourceFilter->Load(wsz.c_str(), NULL);
		if (hRes != S_OK)
			return FALSE;
		DWORD streams = 0;
		hRes = m_asmstream->Count(&streams);
		if (hRes != S_OK)
			return FALSE;
		for (INT i = 0;i < streams;i++)
		{
			hRes = m_asmstream->Enable(i, AMSTREAMSELECTENABLE_ENABLE);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwFlags;
			WCHAR *wszName = NULL;
			ScopedMediaType mediaType;
			hRes = m_asmstream->Info(i, mediaType.Receive(), &dwFlags, NULL, NULL, &wszName, NULL, NULL);
			if (hRes != S_OK)
			{
				if (wszName != NULL)
				{
					CoTaskMemFree(wszName);
					wszName = NULL;
				}
				return FALSE;
			}
			if (wszName != NULL)
			{
				CoTaskMemFree(wszName);
				wszName = NULL;
			}
		}
		hRes = m_builder->AddFilter(m_lavFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_lavAudioO = GetPin(m_lavFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_lavAudioO)
			return FALSE;
		hRes = m_builder->AddFilter(m_audioFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_audioI = GetPin(m_audioFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_audioO)
			return FALSE;
		m_audioO = GetPin(m_audioFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_audioO)
			return FALSE;
		hRes = m_builder->AddFilter(m_videoFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_videoI = GetPin(m_videoFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_videoI)
			return FALSE;
		m_videoO = GetPin(m_videoFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_videoO)
			return FALSE;
		hRes = m_builder->Connect(m_lavAudioO, m_audioI);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->Connect(m_lavAudioO, m_audioI);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->Connect(m_lavAudioO, m_videoI);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::Play()
	{


		return TRUE;
	}
	void LAVPlayer::Close()
	{
		m_control.Release();
		m_seeking.Release();
		if (m_builder)
		{
			m_builder->RemoveFilter(m_lavFilter);
			m_builder->RemoveFilter(m_audioFilter);
			m_builder->RemoveFilter(m_videoFilter);
		}
		m_lavAudioO.Release();
		m_videoI.Release();
		m_videoO.Release();
		m_audioI.Release();
		m_audioO.Release();
		m_lavFilter.Release();
		m_audioFilter.Release();
		m_videoFilter.Release();
		m_builder.Release();

	}
	TinyComPtr<IPin> LAVPlayer::GetPin(IBaseFilter* pFilter, PIN_DIRECTION dest, REFGUID category)
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
	BOOL LAVPlayer::GetPinCategory(IPin* pPin, REFGUID category)
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
}

