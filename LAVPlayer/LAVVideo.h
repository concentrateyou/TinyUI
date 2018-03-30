#pragma once
#include "LAVCommon.h"
#include "LAVVideoFilter.h"

namespace LAV
{
	class LAVVideo : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideo)
	public:
		LAVVideo(IGraphBuilder* builder, IPin* sourceVideoO);
		~LAVVideo();
		BOOL Initialize();
		void Uninitialize();
		void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	private:
		BOOL InitializeVideo();
	private:
		IPin*									m_sourceVideoO;
		IGraphBuilder*							m_builder;
		TinyComPtr<IPin>						m_videoO;
		TinyComPtr<IPin>						m_videoI;
		TinyComPtr<IPin>						m_sinkI;
		TinyComPtr<IBaseFilter>					m_videoFilter;// ”∆µΩ‚¬Î
		TinyScopedReferencePtr<LAVVideoFilter>	m_sinkFilter;
	};
}

