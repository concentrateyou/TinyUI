#pragma once
#include "LAVCommon.h"
#include "LAVAudioFilter.h"

namespace LAV
{
	class LAVAudio : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVAudio)
	public:
		LAVAudio(IGraphBuilder* builder,IPin* sourceAudioO);
		~LAVAudio();
		BOOL Initialize();
		void Uninitialize();
		void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	private:
		BOOL InitializeAudio();
	private:
		IPin*									m_sourceAudioO;
		IGraphBuilder*							m_builder;
		TinyComPtr<IPin>						m_audioO;
		TinyComPtr<IPin>						m_audioI;
		TinyComPtr<IPin>						m_sinkI;
		TinyComPtr<IBaseFilter>					m_audioFilter;//“Ù∆µΩ‚¬Î
		TinyScopedReferencePtr<LAVAudioFilter>	m_sinkFilter;
	};
}


