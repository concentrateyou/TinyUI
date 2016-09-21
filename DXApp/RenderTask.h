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
	DWORD				Render();//∑µªÿ‰÷»æµƒ∫¡√Î ˝
	BOOL				Submit();
	void				Exit() OVERRIDE;
	VideoCapture*		GetCapture();
	BYTE*				GetPointer() const;
	VideoCaptureParam*	GetParam();
private:
	void				OnMessagePump();
	void				OnExit();
private:
	DWORD								m_dwFPS;
	VideoCapture						m_videoCapture;
	VideoCaptureParam					m_videoParam;
	VideoCapture::Name					m_deviceName;
	DXGraphics							m_graphics;
	DX11Image							m_image;
	TinyScopedPtr<DX11CaptureTask>		m_dx11CaptureTask;
	TinyPerformanceTimer				m_timer;
};

