#pragma once
#include "DXFramework.h"
#include "DXGraphics.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
using namespace TinyUI::IO;
using namespace Media;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask();
	virtual	~RenderTask();
	BOOL				Initialize(HWND hWND, INT cx, INT cy, DWORD dwFPS, const VideoCapture::Name& name, const VideoCaptureParam& param);
	DWORD				Render();
	BOOL				Submit();
	void				Exit() OVERRIDE;
	VideoCapture*		GetCapture();
	BYTE*				GetPointer();
	VideoCaptureParam*	GetParam();
private:
	void				OnVideo(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
	void				OnMessagePump();
	void				OnExit();
private:
	DWORD								m_dwFPS;
	VideoCapture						m_capture;
	VideoCaptureParam					m_videoParam;
	VideoCapture::Name					m_deviceName;
	DXGraphics							m_graphics;
	DX11Image							m_image;
	TinyScopedPtr<DX11CaptureTask>		m_dx11CaptureTask;
	TinyPerformanceTimer				m_timer;
	LONG								m_size;
	TinyScopedArray<BYTE>				m_bits;
	IO::TinyRingQueue					m_queue;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)> m_videoCB;
};

