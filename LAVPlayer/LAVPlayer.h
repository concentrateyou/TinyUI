#pragma once
#include "LAVAudioFilter.h"
#include "LAVVideoFilter.h"
#include "LAVVideo.h"
#include "LAVAudio.h"
#include "DX9Graphics2D.h"
#include "DX9Image2D.h"
#include "DX9RenderView.h"
#include "Media/TinyXAudio.h"
using namespace std;
using namespace TinyUI;
using namespace DShow;
using namespace DXFramework;
using namespace TinyUI::Media;

namespace TinyUI
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
		~LAVPlayer();
		BOOL Open(HWND hWND,LPCSTR pzFile);
		BOOL Play();
		void Close();
		LAVAudio* GetAudio();
		LAVVideo* GetVideo();
	private:
		BOOL Initialize();
		void Uninitialize();
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter);
		void OnVideo(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter);
	private:
		HWND										m_hWND;
		TinyScopedPtr<LAVAudio>						m_audio;
		TinyScopedPtr<LAVVideo>						m_video;
		TinyComPtr<IPin>							m_lavAudioO;
		TinyComPtr<IPin>							m_lavVideoO;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IMediaSeeking>					m_seeking;
		TinyComPtr<IAMStreamSelect>					m_asmstream;
		TinyComPtr<IBaseFilter>						m_lavFilter;//数据源
		TinyComPtr<IGraphBuilder>					m_builder;
		DX9Graphics2D								m_graphics;
		DX9Image2D									m_image;
		TinyXAudio									m_xaudio;
		TinyPerformanceTime							m_time;
		TinyPerformanceTimer						m_timer;
	};
}


