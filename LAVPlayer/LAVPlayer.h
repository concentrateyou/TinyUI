#pragma once
#include "LAVAudioFilter.h"
#include "LAVVideoFilter.h"
#include "LAVVideo.h"
#include "LAVAudio.h"
using namespace std;
using namespace TinyUI;
using namespace DShow;

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
	class LAVPlayer : public FilterObserver
	{
		DISALLOW_COPY_AND_ASSIGN(LAVPlayer)
	public:
		LAVPlayer();
		~LAVPlayer();
		BOOL Open(LPCSTR pzFile);
		BOOL Play();
		void Close();
	public:
		static BOOL GetFilterByCLSID(const string& clsid, IBaseFilter** ps);
	private:
		BOOL Initialize();
	private:
		TinyScopedPtr<LAVAudio>					m_audio;
		TinyScopedPtr<LAVVideo>					m_video;
		TinyComPtr<IPin>						m_lavAudioO;
		TinyComPtr<IPin>						m_lavVideoO;
		TinyComPtr<IMediaControl>				m_control;
		TinyComPtr<IMediaSeeking>				m_seeking;
		TinyComPtr<IAMStreamSelect>				m_asmstream;
		TinyComPtr<IBaseFilter>					m_lavFilter;//数据源
		TinyComPtr<IGraphBuilder>				m_builder;
	};
}


