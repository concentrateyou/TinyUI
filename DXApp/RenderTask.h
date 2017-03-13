#pragma once
#include "DXFramework.h"
#include "DX11Graphics2D.h"
#include "DX11CaptureTask.h"
#include "VideoCapture.h"
#include "Utility.h"
#include "Control/TinyMenu.h"
using namespace DXFramework;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace DShow;

namespace DXApp
{
	class DXWindow;

	class RenderTask : public TinyTaskBase, public GraphicsObserver
	{
	public:
		RenderTask();
		virtual	~RenderTask();
		DX11Graphics2D&	GetGraphics();
		BOOL			Initialize(DXWindow* pWindow, INT cx, INT cy, DWORD dwFPS);
		LONGLONG		Render();
		BOOL			Submit();
		BOOL			Contain(DX11Element2D* element);
		BOOL			Add(DX11Element2D* element);
		void			Remove(DX11Element2D* element);
		void			BringToTop(DX11Element2D* element);
		void			BringToBottom(DX11Element2D* element);
		void			MoveUp(DX11Element2D* element);
		void			MoveDown(DX11Element2D* element);
		DX11Element2D*	HitTest(const TinyPoint& pos);
	public:
		void			OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		BOOL			Close(DWORD dwMs = INFINITE) OVERRIDE;
	private:
		void			OnMessagePump();
		void			OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void			OnMenuClick(void*, INT wID);
	private:
		BOOL						m_bMouseTracking;
		TinyMenu					m_menu;
		DXWindow*					m_pWindow;
		DWORD						m_dwFPS;
		DX11Graphics2D				m_graphics;
		TinyEvent					m_close;
		TinyPerformanceTimer		m_timer;
		TinyArray<DX11Element2D*>	m_scenes;
		DX11Element2D*				m_lastElement;
		DX11Rectangle2D				m_handles[8];
	private:
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSize;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onRButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseLeave;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
		TinyScopedPtr<Delegate<void(void*, INT)>>				   m_onMenuClick;
	};


}

