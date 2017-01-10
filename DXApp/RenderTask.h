#pragma once
#include "DXFramework.h"
#include "DX11Graphics2D.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
using namespace DXFramework;
using namespace TinyUI::IO;
using namespace DShow;

class DXWindow;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask();
	virtual	~RenderTask();
	DX11Graphics2D*		GetGraphics();
	BOOL				Initialize(DXWindow* pWindow, INT cx, INT cy, DWORD dwFPS);
	DWORD				Render();
	BOOL				Submit();
	BOOL				Close(DWORD dwMs = INFINITE) OVERRIDE;
	virtual void		OnRender(BYTE* bits, LONG size, FLOAT ts);
	BOOL				Contain(DX11Element* element);
	BOOL				Add(DX11Element* element);
	void				Remove(DX11Element* element);
public:
	void				OnMessagePump();
	void				OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void				OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void				OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void				OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void				OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	Event<void(BYTE*, LONG, FLOAT)> EVENT_RENDER;
private:
	DXWindow*				m_pWindow;
	DWORD					m_dwFPS;
	DX11Graphics2D			m_graphics;
	TinyEvent				m_close;
	TinyPerformanceTimer	m_timer;
	TinyArray<DX11Element*>	m_scenes;
	TinyLock				m_lock;
	DX11Element*			m_currentElement;
private:
	TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSize;
	TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
	TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
	TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
	TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
};

