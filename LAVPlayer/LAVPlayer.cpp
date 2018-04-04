#include "stdafx.h"
#include "LAVPlayer.h"

namespace LAV
{
	LAVClock::LAVClock()
		:m_clock(NAN)
	{
		ZeroMemory(&m_clockFreq, sizeof(m_clockFreq));
		QueryPerformanceFrequency(&m_clockFreq);
	}
	void LAVClock::SetClock(DOUBLE clock)
	{
		m_clock = clock;
	}

	DOUBLE LAVClock::GetClock()
	{
		return m_clock;
	}
	QWORD LAVClock::GetQPCTimeMS()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		QWORD timeVal = currentTime.QuadPart;
		timeVal *= 1000;
		timeVal /= m_clockFreq.QuadPart;
		return timeVal;
	};

	//////////////////////////////////////////////////////////////////////////
	LAVPlayer::LAVPlayer()
		:m_hWND(NULL)
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
	LAVAudio* LAVPlayer::GetAudio()
	{
		return m_audio;
	}
	LAVVideo* LAVPlayer::GetVideo()
	{
		return m_video;
	}
	BOOL LAVPlayer::Open(HWND hWND, LPCSTR pzFile)
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
				if (!m_video->Initialize(BindCallback(&LAVPlayer::OnVideo, this)))
					return FALSE;
				AM_MEDIA_TYPE mediaType;
				ZeroMemory(&mediaType, sizeof(mediaType));
				if (!m_video->GetMediaType(&mediaType))
					return FALSE;
				VIDEOINFOHEADER vih = { 0 };
				memcpy(&vih, mediaType.pbFormat, sizeof(vih));
				FreeMediaType(mediaType);
				TinyRectangle s;
				GetClientRect(hWND, &s);
				if (!m_graphics.Initialize(hWND, s.Size()))
					return FALSE;
				m_graphics.SetRenderView(NULL);
				m_image.Destory();
				if (!m_image.Create(m_graphics.GetDX9(), vih.bmiHeader.biWidth, vih.bmiHeader.biHeight, NULL))
					return FALSE;
				m_image.SetScale(s.Size());
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
				if (!m_audio->Initialize(BindCallback(&LAVPlayer::OnAudio, this)))
					return FALSE;
				AM_MEDIA_TYPE mediaType;
				ZeroMemory(&mediaType, sizeof(mediaType));
				if (!m_audio->GetMediaType(&mediaType))
					return FALSE;
				WAVEFORMATEX waveFMT = { 0 };
				memcpy(&waveFMT, mediaType.pbFormat, sizeof(waveFMT));
				FreeMediaType(mediaType);
				if (!m_xaudio.Open(&waveFMT))
					return FALSE;
				m_xaudio.Start();
			}
		}
		return TRUE;
	}
	BOOL LAVPlayer::Play()
	{
		if (m_control != NULL)
			return SUCCEEDED(m_control->Run());
		return FALSE;
	}
	void LAVPlayer::Close()
	{
		if (m_control != NULL)
			m_control->Stop();
		m_clock.SetClock(NAN);
		m_image.Destory();
		m_xaudio.Stop();
		m_xaudio.Close();
		Uninitialize();
	}
	void LAVPlayer::OnAudio(BYTE* bits, LONG size, FLOAT time, LPVOID lpParameter)
	{
		LONGLONG msQPC = m_clock.GetQPCTimeMS();
		m_clock.SetClock(static_cast<DOUBLE>(time));
		m_xaudio.Play(bits, size, 5000);
		m_clock.SetClock(m_clock.GetClock() + (m_clock.GetQPCTimeMS() - msQPC));
	}
	BOOL LAVPlayer::CopyVideo(BYTE* bits, LONG size)
	{
		if (!m_image.Copy(bits, size))
			return FALSE;
		if (!m_graphics.GetRenderView()->BeginDraw())
			return FALSE;
		if (!m_graphics.DrawImage(&m_image, 1.0, 1.0, 180))
			return FALSE;
		if (!m_graphics.GetRenderView()->EndDraw())
			return FALSE;
		return TRUE;
	}
	void LAVPlayer::OnVideo(BYTE* bits, LONG size, FLOAT time, LPVOID lpParameter)
	{
		if (CopyVideo(bits, size))
		{
			while (isnan(m_clock.GetClock()));
			INT delay = static_cast<INT>(time - m_clock.GetClock());
			if (m_timer.Waiting(delay, 100))
			{
				m_graphics.Present();
			}
		}
	}
}

