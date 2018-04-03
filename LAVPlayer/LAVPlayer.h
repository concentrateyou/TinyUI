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
	/// https://github.com/Nevcairiel/LAVFilters
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375655(v=vs.85).aspx
	/// ����LAV��ȫ�ܲ�����
	/// DirectShow Filters(DirectShow Filters)
	/// </summary>
	class LAVPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(LAVPlayer)
	public:
		LAVPlayer();
		~LAVPlayer();
		BOOL Open(HWND hWND, LPCSTR pzFile);
		BOOL Play();
		void Close();
		LAVAudio* GetAudio();
		LAVVideo* GetVideo();
	private:
		BOOL Initialize();
		void Uninitialize();
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter);
		void OnVideo(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter);
		BOOL CopyVideo(BYTE* bits, LONG size);
	private:
		HWND										m_hWND;
		LAVClock									m_clock;
		TinyScopedPtr<LAVAudio>						m_audio;
		TinyScopedPtr<LAVVideo>						m_video;
		TinyComPtr<IPin>							m_lavAudioO;
		TinyComPtr<IPin>							m_lavVideoO;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IMediaSeeking>					m_seeking;
		TinyComPtr<IAMStreamSelect>					m_asmstream;
		TinyComPtr<IBaseFilter>						m_lavFilter;//����Դ
		TinyComPtr<IGraphBuilder>					m_builder;
		DX9Graphics2D								m_graphics;
		DX9Image2D									m_image;
		TinyXAudio									m_xaudio;
		TinyPerformanceTimer						m_timer;
	};
}


