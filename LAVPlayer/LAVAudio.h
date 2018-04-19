#pragma once
#include "LAVCommon.h"
#include "LAVAudioFilter.h"
#include "LAVAudioSettings.h"

namespace LAV
{
	class LAVAudio : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVAudio)
	public:
		LAVAudio(IGraphBuilder* builder, IPin* lavAudioO);
		~LAVAudio();
		BOOL Initialize(Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback);
		void Uninitialize();
		BOOL GetMediaType(AM_MEDIA_TYPE* pType);
		BOOL GetAudioSettings(ILAVAudioSettings* settings);
	public:
		BOOL GetOutputMediaTypes(TinyArray<ScopedMediaType>& mediaTypes);
		void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	public:
		static BOOL GetMediaTypes(IPin* pPin, TinyArray<ScopedMediaType>& mediaTypes);
	private:
		BOOL InitializeAudio();
	private:
		IPin*										m_lavAudioO;
		IGraphBuilder*								m_builder;
		TinyComPtr<IPin>							m_audioO;
		TinyComPtr<IPin>							m_audioI;
		TinyComPtr<IPin>							m_sinkI;
		TinyComPtr<IBaseFilter>						m_audioFilter;//��Ƶ����
		TinyScopedReferencePtr<LAVAudioFilter>		m_sinkFilter;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_callback;
	};
}

