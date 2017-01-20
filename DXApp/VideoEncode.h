#pragma once
#include "DXFramework.h"
#include "VideoCapture.h"
#include "x264Encode.h"
#include "I420Converter.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace DShow;


namespace DXApp
{
	class RenderTask;

	class VideoEncode : public TinyTaskBase
	{
	public:
		VideoEncode(RenderTask* renderTask);
		virtual ~VideoEncode();
		BOOL				Initialize(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate);
		BOOL				Submit();
		BOOL				Close(DWORD dwMS = INFINITE) OVERRIDE;
		x264Encode&			GetEncode();
		TinySize			GetSize() const;
		DWORD				GetFPS() const;
	private:
		DWORD				Encode();
		void				OnMessagePump();
	private:
		TinyPerformanceTimer			m_timer;
		TinyEvent						m_close;
		DWORD							m_dwFPS;
		DWORD							m_dwVideoRate;
		RenderTask*						m_renderTask;
		x264Encode						m_x264;
		TinyScopedPtr<I420Converter>	m_converter;
	};
}

