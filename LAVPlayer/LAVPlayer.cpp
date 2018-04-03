#include "stdafx.h"
#include "LAVPlayer.h"

namespace LAV
{
	LAVPlayer::LAVPlayer()
	{
	}

	LAVPlayer::~LAVPlayer()
	{
		this->Close();
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
		if (!GetFilterByCLSID("{B98D13E7-55DB-4385-A33D-09FD1BA26338}", &m_lavFilter))
			return FALSE;
		hRes = m_builder->AddFilter(m_lavFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	void LAVPlayer::Uninitialize()
	{
		if (m_builder != NULL)
		{
			m_builder->Disconnect(m_lavAudioO);
			m_builder->Disconnect(m_lavVideoO);
			m_builder->RemoveFilter(m_lavFilter);
		}
		m_control.Release();
		m_seeking.Release();
		m_asmstream.Release();
		m_lavAudioO.Release();
		m_lavVideoO.Release();
		m_lavFilter.Release();
		if (m_audio != NULL)
			m_audio->Uninitialize();
		m_audio.Reset(NULL);
		if (m_video != NULL)
			m_video->Uninitialize();
		m_video.Reset(NULL);
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
				if (!m_video->Initialize())
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
				if (!m_audio->Initialize())
					return FALSE;
			}
		}
		return TRUE;
	}
	BOOL LAVPlayer::Play()
	{
		HRESULT hRes = S_OK;
		if (m_control != NULL)
		{
			hRes = m_control->Run();
		}
		return hRes == S_OK;
	}
	void LAVPlayer::Close()
	{
		if (m_control != NULL)
			m_control->Stop();
		Uninitialize();
	}
}

