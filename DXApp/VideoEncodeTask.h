#pragma once
#include "DXFramework.h"
#include "DXGraphics.h"
#include "VideoCapture.h"
#include "x264Encode.h"
#include "I420Converter.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class RenderTask;

class VideoEncodeTask : public TinyTaskBase
{
public:
	VideoEncodeTask(RenderTask* renderTask);
	virtual ~VideoEncodeTask();
	BOOL				Open(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate);
	BOOL				Submit();
	BOOL				Close(DWORD dwMS) OVERRIDE;
	x264Encode*			GetEncode();
	VideoCapture*		GetCapture();
	VideoCaptureParam*	GetParam();
private:
	void				OnMessagePump();
	void				OnClose();
	void				OnRender(BYTE* bits, LONG size);
private:
	DWORD							m_dwFPS;
	DWORD							m_dwVideoRate;
	RenderTask*						m_renderTask;
	x264Encode						m_x264;
	TinyScopedPtr<I420Converter>	m_converter;
	TinyPerformanceTimer			m_timer;
	TinyEvent						m_close;
	RawSampleQueue					m_queue;
	TinyScopedPtr<Delegate<void(BYTE*,LONG)>>	m_render;
};

