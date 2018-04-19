#pragma once
#include "LAVCommon.h"
#include "LAVVideoFilter.h"
#include "LAVVideoSettings.h"

namespace LAV
{
	class LAVVideo : public FilterObserver
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideo)
	public:
		LAVVideo(IGraphBuilder* builder, IPin* lavVideoO);
		~LAVVideo();
		BOOL Initialize(Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback);
		void Uninitialize();
		BOOL GetOutputMediaTypes(TinyArray<ScopedMediaType>& mediaTypes);
		BOOL GetMediaType(AM_MEDIA_TYPE* pType);
		BOOL GetVideoSettings(ILAVVideoSettings* settings);
	public:
		void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	private:
		BOOL InitializeVideo();
		static BOOL GetMediaTypes(IPin* pPin, TinyArray<ScopedMediaType>& mediaTypes);
	private:
		IPin*										m_lavVideoO;
		IGraphBuilder*								m_builder;
		TinyComPtr<IPin>							m_videoO;//�������
		TinyComPtr<IPin>							m_videoI;//��������
		TinyComPtr<IPin>							m_sinkI;
		TinyComPtr<IBaseFilter>						m_videoFilter;//��Ƶ����
		TinyComPtr<ILAVVideoSettings>				m_settings;
		TinyScopedReferencePtr<LAVVideoFilter>		m_sinkFilter;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_callback;
	};
}

