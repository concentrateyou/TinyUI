#pragma once
#include "DXFramework.h"
#include "DX11Graphics2D.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
using namespace DXFramework;
using namespace TinyUI::IO;
using namespace DShow;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask();
	virtual	~RenderTask();
	DX11Graphics2D*		GetGraphics();
	BOOL				Initialize(HWND hWND, INT cx, INT cy, DWORD dwFPS);
	DWORD				Render();
	BOOL				Submit();
	BOOL				Close(DWORD dwMs = INFINITE) OVERRIDE;
	virtual void		OnRender(BYTE* bits, LONG size, FLOAT ts);
	BOOL				Add(DX11Element* element);
	void				Remove(DX11Element* element);
public:
	void				OnMessagePump();
public:
	Event<void(BYTE*, LONG, FLOAT)> EVENT_RENDER;
private:
	DWORD								m_dwFPS;
	DX11Graphics2D						m_graphics;
	TinyEvent							m_close;
	TinyPerformanceTimer				m_timer;
	TinyArray<DX11Element*>				m_scenes;
	TinyLock							m_lock;
};

