#pragma once
#include "LAVAudioFilter.h"
#include "LAVVideoFilter.h"
#include "LAVVideo.h"
#include "LAVAudio.h"
#include "DX11.h"
#include "DX11Graphics2D.h"
#include "DX11Image2D.h"
#include "Media/TinyXAudio.h"
using namespace std;
using namespace TinyFramework;
using namespace DShow;
using namespace DXFramework;
using namespace TinyFramework::Media;

namespace TinyFramework
{
	template<class T>
	struct ComMemeryDeleter
	{
		void operator()(T *_Ptr) const _NOEXCEPT
		{
			if (_Ptr != NULL)
			{
				CoTaskMemFree(_Ptr);
			}
		}
	};
}

namespace LAV
{
	class LAVClock
	{
		DISALLOW_COPY_AND_ASSIGN(LAVClock)
	public:
		LAVClock();
		void SetClock(DOUBLE clock);
		DOUBLE GetClock();
		QWORD GetQPCTimeMS();
	private:
		DOUBLE			m_clock;
		LARGE_INTEGER	m_clockFreq;
	};
	/// <summary>
	///无窗口播放器
	/// </summary>
	class LAVWindowlessPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(LAVWindowlessPlayer)
	public:
		LAVWindowlessPlayer();
		virtual ~LAVWindowlessPlayer();
		BOOL Open(LPCSTR pzFile, const LAVVideoFormat& vF, const LAVAudioFormat& aF);
		BOOL Play();
		void Close();
		BOOL GetDuration(LONGLONG& time);
		BOOL GetAvailable(LONGLONG& time1, LONGLONG time2);
		BOOL GetPosition(LONGLONG& pos);
		BOOL SetPosition(LONGLONG pos);
		BOOL SetRate(DOUBLE dRate);
		BOOL GetRate(DOUBLE* pdRate);
		BOOL GetTrackCount(UINT& count);
		BOOL GetTrackType(UINT index, TrackType& type);
		BOOL ShowProperty(HWND hWND);
		LAVAudio* GetAudio();
		LAVVideo* GetVideo();
		const LAVAudioFormat&		GetAudioFormat() const;
		const LAVVideoFormat&		GetVideoFormat() const;
	public:
		Event<void(BYTE*, LONG, REFERENCE_TIME)> EVENT_AUDIO;
		Event<void(BYTE*, LONG, REFERENCE_TIME)> EVENT_VIDEO;
	public:
		static BOOL GetFormats(LPCSTR pzFile, vector<LAVVideoFormat>& videos, vector<LAVAudioFormat>& audios);
	private:
		BOOL Initialize();
		void Uninitialize();
		void OnAudio(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter);
		void OnVideo(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter);
	private:
		LAVVideoFormat								m_vF;
		LAVAudioFormat								m_aF;
		TinyScopedPtr<LAVAudio>						m_audio;
		TinyScopedPtr<LAVVideo>						m_video;
		TinyComPtr<IPin>							m_lavAudioO;
		TinyComPtr<IPin>							m_lavVideoO;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IMediaSeeking>					m_seeking;
		TinyComPtr<IAMStreamSelect>					m_asmstream;
		TinyComPtr<IBaseFilter>						m_lavFilter;//数据源
		TinyComPtr<IGraphBuilder>					m_builder;
	};
	/// <summary>
	/// https://github.com/Nevcairiel/LAVFilters
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375655(v=vs.85).aspx
	/// 基于LAV的全能播放器
	/// DirectShow Filters(DirectShow Filters)
	/// </summary>
	class LAVPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(LAVPlayer)
	public:
		LAVPlayer();
		virtual ~LAVPlayer();
		BOOL Open(HWND hWND, LPCSTR pzFile, const LAVVideoFormat& vF, const LAVAudioFormat& aF);
		BOOL Play();
		void Close();
		BOOL GetDuration(LONGLONG& time);
		BOOL GetAvailable(LONGLONG& time1, LONGLONG time2);
		BOOL GetPosition(LONGLONG& pos);
		BOOL SetPosition(LONGLONG pos);
		BOOL SetRate(DOUBLE dRate);
		BOOL GetRate(DOUBLE* pdRate);
		BOOL GetTrackCount(UINT& count);
		BOOL GetTrackType(UINT index, TrackType& type);
		BOOL ShowProperty(HWND hWND);
		LAVAudio* GetAudio();
		LAVVideo* GetVideo();
	private:
		BOOL Initialize();
		void Uninitialize();
		void OnAudio(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter);
		void OnVideo(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter);
		BOOL Copy(BYTE* bits, LONG size);
	private:
		HWND										m_hWND;
		LAVClock									m_clock;
		DX11										m_dx11;
		DX11Graphics2D								m_graphics;
		DX11Image2D									m_image;
		TinyXAudio									m_xaudio;
		TinyPerformanceTimer						m_timer;
		TinyScopedPtr<LAVAudio>						m_audio;
		TinyScopedPtr<LAVVideo>						m_video;
		TinyComPtr<IPin>							m_lavAudioO;
		TinyComPtr<IPin>							m_lavVideoO;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IMediaSeeking>					m_seeking;
		TinyComPtr<IAMStreamSelect>					m_asmstream;
		TinyComPtr<IBaseFilter>						m_lavFilter;//数据源
		TinyComPtr<IGraphBuilder>					m_builder;
	};
}


