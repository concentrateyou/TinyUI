#pragma once
#include "LAVCommon.h"
#include "lav/LAVVideoSettings.h"
#include "LAVVideoFilter.h"

namespace LAV
{
	class LAVVideo : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideo)
	public:
		LAVVideo(IGraphBuilder* builder, IPin* lavVideoO);
		~LAVVideo();
		BOOL	Initialize();
		BOOL	Initialize(Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>&& callback);
		void	Uninitialize();
		BOOL	GetOutputMediaTypes(TinyArray<ScopedMediaType>& mediaTypes);
		BOOL	GetMediaType(AM_MEDIA_TYPE* pType);
		BOOL	GetVideoSettings(ILAVVideoSettings* settings);
	public:
		void	OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter) OVERRIDE;
	private:
		BOOL	InitializeVideo();
		static BOOL GetMediaTypes(IPin* pPin, TinyArray<ScopedMediaType>& mediaTypes);
	private:
		IPin*												m_lavVideoO;
		IGraphBuilder*										m_builder;
		TinyComPtr<IPin>									m_videoO;//解码输出
		TinyComPtr<IPin>									m_videoI;//解码输入
		TinyComPtr<IPin>									m_sinkI;
		TinyComPtr<IBaseFilter>								m_videoFilter;//视频解码
		TinyComPtr<ILAVVideoSettings>						m_settings;
		TinyScopedReferencePtr<LAVVideoFilter>				m_sinkFilter;
		Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>	m_callback;
	};
}

