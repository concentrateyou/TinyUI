#pragma once
#include "LAVAudioFilter.h"
#include "LAVVideoFilter.h"
#include <dshow.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
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
	class LAVPlayer
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
		BOOL InitializeLAVS();
		TinyComPtr<IPin> GetPin(IBaseFilter* pFilter, PIN_DIRECTION dest, REFGUID category);
	private:
		static BOOL GetPinCategory(IPin* pPin, REFGUID category);
	private:
		TinyComPtr<IPin>						m_lavAudioO;TinyComPtr<IPin>						m_lavVideoO;
		TinyComPtr<IPin>						m_audioO;
		TinyComPtr<IPin>						m_audioI;
		TinyComPtr<IPin>						m_videoO;
		TinyComPtr<IPin>						m_videoI;
		TinyComPtr<IMediaControl>				m_control;
		TinyComPtr<IMediaSeeking>				m_seeking;
		TinyComPtr<IAMStreamSelect>				m_asmstream;
		TinyComPtr<IBaseFilter>					m_lavFilter;//数据源
		TinyComPtr<IBaseFilter>					m_audioFilter;//音频解码
		TinyComPtr<IBaseFilter>					m_videoFilter;//视频解码
		TinyComPtr<IGraphBuilder>				m_builder;
		TinyScopedReferencePtr<LAVAudioFilter>	m_lavAudioFilter;
		TinyScopedReferencePtr<LAVVideoFilter>	m_lavVideoFilter;
	};
}


