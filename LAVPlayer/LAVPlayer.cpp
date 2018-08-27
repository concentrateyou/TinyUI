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
		:m_hWND(NULL),
		m_graphics(m_dx10)
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
		for (DWORD i = 0; i < streams; i++)
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
				if (!m_dx10.Initialize(hWND, TO_CX(s), TO_CY(s)))
					return FALSE;
				if (!m_graphics.Create())
					return FALSE;
				if (!m_graphics.InitializeShaders())
					return FALSE;
				m_image.Destory();
				if (!m_image.Create(m_graphics.GetDX10(), vih.bmiHeader.biWidth, vih.bmiHeader.biHeight, NULL, FALSE))
					return FALSE;
				m_image.SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(s)) / m_image.GetSize().x, static_cast<FLOAT>(TO_CY(s)) / m_image.GetSize().y));
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
		{
			if (SUCCEEDED(m_control->Run()))
			{
				m_graphics.BeginDraw();
				m_graphics.EndDraw();
				m_dx10.Present();
				return TRUE;
			}
		}
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
	BOOL LAVPlayer::GetDuration(LONGLONG& time)
	{
		time = 0;
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->GetDuration(&time)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::GetAvailable(LONGLONG& time1, LONGLONG time2)
	{
		time1 = 0;
		time2 = 0;
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->GetAvailable(&time1, &time2)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::GetPosition(LONGLONG& pos)
	{
		pos = 0;
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->GetPositions(&pos, NULL)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::SetPosition(LONGLONG pos)
	{
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_AbsolutePositioning)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::SetRate(DOUBLE dRate)
	{
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->SetRate(dRate)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::GetRate(DOUBLE* pdRate)
	{
		if (!pdRate)
			return FALSE;
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<IMediaSeeking> seeking;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(IMediaSeeking), (void **)&seeking)))
			return FALSE;
		if (FAILED(seeking->GetRate(pdRate)))
			return FALSE;
		return TRUE;
	}
	BOOL LAVPlayer::GetTrackCount(UINT& count)
	{
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<ITrackInfo> info;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(ITrackInfo), (void **)&info)))
			return FALSE;
		count = info->GetTrackCount();
		return TRUE;
	}
	BOOL LAVPlayer::GetTrackType(UINT index, TrackType& type)
	{
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<ITrackInfo> info;
		HRESULT hRes = m_lavFilter->QueryInterface(__uuidof(ITrackInfo), (void **)&info);
		if (hRes != S_OK)
			return FALSE;
		TrackElement element = { 0 };
		if (info->GetTrackInfo(index, &element))
		{
			type = (TrackType)element.Type;
			return TRUE;
		}
		return FALSE;
	}
	BOOL LAVPlayer::ShowProperty(HWND hWND)
	{
		if (!m_lavFilter)
			return FALSE;
		TinyComPtr<ISpecifyPropertyPages> pages;
		if (FAILED(m_lavFilter->QueryInterface(__uuidof(ISpecifyPropertyPages), (void **)&pages)))
			return FALSE;
		CAUUID cauuid;
		if (FAILED(pages->GetPages(&cauuid)))
			return FALSE;
		if (cauuid.cElems > 0)
		{
			FILTER_INFO filterInfo;
			TinyComPtr<IUnknown> unknow;
			if (SUCCEEDED(m_lavFilter->QueryFilterInfo(&filterInfo)) &&
				SUCCEEDED(m_lavFilter->QueryInterface(IID_IUnknown, (void **)&unknow)))
			{
				OleCreatePropertyFrame(hWND, 0, 0, filterInfo.achName, 1, &unknow, cauuid.cElems, cauuid.pElems, 0, 0, NULL);
			}
			CoTaskMemFree(cauuid.pElems);
			filterInfo.pGraph->Release();
			return TRUE;
		}
		return FALSE;
	}
	void LAVPlayer::OnAudio(BYTE* bits, LONG size, REFERENCE_TIME time, LPVOID lpParameter)
	{
		LONGLONG msQPC = m_clock.GetQPCTimeMS();
		m_clock.SetClock(static_cast<DOUBLE>(time));
		m_xaudio.Play(bits, size, 5000);
		m_clock.SetClock(m_clock.GetClock() + (m_clock.GetQPCTimeMS() - msQPC));
	}
	BOOL LAVPlayer::Copy(BYTE* bits, LONG size)
	{
		INT cy = static_cast<INT>(m_image.GetSize().y);
		INT linesize = size / cy;
		if (!m_image.Copy(m_dx10, bits, size, linesize))
			return FALSE;
		if (!m_graphics.BeginDraw())
			return FALSE;
		m_image.SetFlipH(FALSE);
		m_image.SetFlipV(TRUE);
		if (!m_graphics.DrawImage(m_image))
			return FALSE;
		if (!m_graphics.EndDraw())
			return FALSE;
		return TRUE;
	}
	void LAVPlayer::OnVideo(BYTE* bits, LONG size, REFERENCE_TIME time, LPVOID lpParameter)
	{
		if (Copy(bits, size))
		{
			while (isnan(m_clock.GetClock()));
			INT delay = static_cast<INT>(time - m_clock.GetClock());
			if (m_timer.Waiting(delay, 80))
			{
				m_dx10.Present();
			}
		}
	}
}

