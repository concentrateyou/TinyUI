#pragma once
#include "DXFramework.h"
#include "DXGraphics.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
using namespace TinyUI::IO;
using namespace DShow;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask();
	virtual	~RenderTask();
	BOOL				Initialize(HWND hWND, INT cx, INT cy, DWORD dwFPS);
	DWORD				Render();
	BOOL				Submit();
	VideoCapture*		GetCapture();
	VideoCaptureParam*	GetParam();
	BYTE*				GetPointer();
	BOOL				Close(DWORD dwMs = INFINITE) OVERRIDE;
	virtual void		OnRender(BYTE* bits, LONG size, FLOAT ts);
public:
	Event<void(BYTE*, LONG, FLOAT)> EVENT_RENDER;
	void				OnMessagePump();
	void				OnExit();
private:
	DWORD								m_dwFPS;
	DXGraphics							m_graphics;
	TinyEvent							m_close;
	TinyPerformanceTimer				m_timer;
	TinyArray<DX11Element*>				m_scenes;
};

