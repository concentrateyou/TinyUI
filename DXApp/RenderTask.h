#pragma once
#include "DXFramework.h"
#include "DX11Graphics2D.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
#include "Control/TinyMenu.h"
using namespace DXFramework;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace DShow;

namespace DXApp
{
	class DXWindow;

	class RenderTask : public TinyTaskBase
	{
	public:
		RenderTask();
		virtual	~RenderTask();
		DX11Graphics2D*	GetGraphics();
		BOOL			Initialize(DXWindow* pWindow, INT cx, INT cy, DWORD dwFPS);
		DWORD			Render();
		BOOL			Submit();
		BOOL			Close(DWORD dwMs = INFINITE) OVERRIDE;
		virtual void	OnRender(BYTE* bits, LONG size, FLOAT ts);
		BOOL			Contain(DX11Element* element);
		BOOL			Add(DX11Element* element);
		void			Remove(DX11Element* element);
		void			BringToTop(DX11Element* element);
		void			BringToBottom(DX11Element* element);
		void			MoveUp(DX11Element* element);
		void			MoveDown(DX11Element* element);
		DX11Element*	HitTest(const TinyPoint& pos);
	private:
		void			OnMessagePump();
		void			OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnMenuClick(void*, INT wID);
	public:
		Event<void(BYTE*, LONG, FLOAT)> EVENT_RENDER;
	private:
		TinyMenu					m_menu;
		DXWindow*					m_pWindow;
		DWORD						m_dwFPS;
		DX11Graphics2D				m_graphics;
		TinyEvent					m_close;
		TinyPerformanceTimer		m_timer;
		TinyArray<DX11Element*>		m_scenes;
		TinyLock					m_lock;
		DX11Element*				m_lastElement;
	private:
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSize;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onRButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
		TinyScopedPtr<Delegate<void(void*, INT)>>				   m_onMenuClick;
	};


}

