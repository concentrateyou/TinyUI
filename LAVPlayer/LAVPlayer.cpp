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
		for (DWORD i = 0;i < streams;i++)
		{
			DWORD dwFlags;
			TinyScopedPtr<WCHAR, ComMemeryDeleter<WCHAR>> wszName;
			ScopedMediaType mediaType;
			hRes = m_asmstream->Info(i, mediaType.Receive(), &dwFlags, NULL, NULL, &wszName, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			if (mediaType->majortype == MEDIATYPE_Video)
			{
				hRes = m_asmstream->Enable(i, AMSTREAMSELECTENABLE_ENABLE);
				if (hRes != S_OK)
					return FALSE;
				hRes = m_lavFilter->FindPin(L"Video", &m_lavVideoO);
				if (hRes != S_OK)
					return FALSE;
				m_video.Reset(new LAVVideo(m_builder, m_lavVideoO));
				if (!m_video)
					return FALSE;
			}
			if (mediaType->majortype == MEDIATYPE_Audio)
			{
				hRes = m_asmstream->Enable(i, AMSTREAMSELECTENABLE_ENABLE);
				if (hRes != S_OK)
					return FALSE;
				hRes = m_lavFilter->FindPin(L"Audio", &m_lavAudioO);
				if (hRes != S_OK)
					return FALSE;
				m_audio.Reset(new LAVAudio(m_builder, m_lavAudioO));
				if (!m_audio)
					return FALSE;
			}
		}
		return TRUE;
	}
	BOOL LAVPlayer::Play()
	{

		return TRUE;
	}
	void LAVPlayer::Close()
	{

	}
}

