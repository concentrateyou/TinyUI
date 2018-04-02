#pragma once
#include "LAVCommon.h"
#include "LAVVideoFilter.h"

namespace LAV
{
	class LAVVideo : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideo)
	public:
		LAVVideo(IGraphBuilder* builder, IPin* lavVideoO);
		~LAVVideo();
		BOOL Initialize();
		void Uninitialize();
		BOOL GetInputMediaType(AM_MEDIA_TYPE** ppType);
		BOOL GetOutputMediaType(AM_MEDIA_TYPE** ppType);
	public:
		void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	private:
		BOOL InitializeVideo();
		static BOOL GetMediaType(IPin* pPin, AM_MEDIA_TYPE** ppType);
	private:
		IPin*									m_lavVideoO;
		IGraphBuilder*							m_builder;
		TinyComPtr<IPin>						m_videoO;
		TinyComPtr<IPin>						m_videoI;
		TinyComPtr<IPin>						m_sinkI;
		TinyComPtr<IBaseFilter>					m_videoFilter;// ”∆µΩ‚¬Î
		TinyScopedReferencePtr<LAVVideoFilter>	m_sinkFilter;
	};
}

