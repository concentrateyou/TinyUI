#include "../stdafx.h"
#include "../Common/TinyApplication.h"
#include "TinyControl.h"
#include "TinyMenu.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyControl, TinyWindow);

	TinyControl::TinyControl()
		:m_pMenu(NULL)
	{

	}
	TinyControl::~TinyControl()
	{
	}
	LRESULT CALLBACK TinyControl::CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam)
	{
		if (code == HCBT_CREATEWND)
		{
			ASSERT(lParam != NULL);
			LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
			ASSERT(lpcs != NULL);
			TinyControl *_this = NULL;
			_this = (TinyControl*)(lpcs->lpCreateParams);
			if (_this != NULL)
			{
				_this->SubclassWindow((HWND)wParam);
			}
		}
		return CallNextHookEx(m_hhk, code, wParam, lParam);
	};
	BOOL TinyControl::Create(HWND hParent, INT x, INT y, INT cx, INT cy, BOOL bHook)
	{
		BOOL bRes = FALSE;
		if (bHook)
		{
			m_hhk = SetWindowsHookEx(WH_CBT, CbtFilterHook, TinyApplication::GetInstance()->Handle(), GetCurrentThreadId());
			if (m_hhk != NULL && TinyWindow::Create(hParent, x, y, cx, cy))
			{
				::SendMessage(m_hWND, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
				UnhookWindowsHookEx(m_hhk);
				m_hhk = NULL;
				return TRUE;
			}
			return FALSE;
		}
		return TinyWindow::Create(hParent, x, y, cx, cy);
	};
	LPCSTR TinyControl::RetrieveClassName()
	{
		return TEXT("TinyControl");
	}
	HICON TinyControl::RetrieveIcon()
	{
		return NULL;
	}
	LPCTSTR TinyControl::RetrieveTitle()
	{
		return TEXT("TinyControl");
	}
	DWORD TinyControl::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CHILD);
	}
	DWORD TinyControl::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);
	}
	LRESULT TinyControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_CREATE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_DESTORY(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_SETCURSOR(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (lParam != NULL)
		{
			return ::SendMessage((HWND)lParam, (WM_COMMANDREFLECT), wParam, lParam);
		}
		else
		{
			bHandled = FALSE;
			//²Ëµ¥ClickÊÂ¼þ
			INT wID = LOWORD(wParam);
			if (m_pMenu != NULL)
			{
				m_pMenu->OnClick(m_pMenu, wID);
			}
			return TRUE;
		}
		return FALSE;
	}
	LRESULT TinyControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_SIZE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyControl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_MOUSEMOVE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_MOUSELEAVE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LBUTTONDOWN(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LBUTTONUP(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LDBCLICK(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_RBUTTONDOWN(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_RBUTTONUP(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		EVENT_RDBCLICK(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyControl::OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyControl::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyControl::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPNMHDR p = (LPNMHDR)lParam;
		return ::SendMessage(p->hwndFrom, (uMsg + WM_REFLECT), wParam, lParam);
	}
	LRESULT TinyControl::OnCommandReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnNotifyReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnDeleteItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnDrawItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnMeasureItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnCut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnClear(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyControl::OnIniMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyHandleHMENU** pps = TinyApplication::GetInstance()->GetMapHMENU().Lookup(reinterpret_cast<HMENU>(wParam));
		if (pps != NULL)
		{
			m_pMenu = static_cast<TinyMenu*>(*pps);
		}
		return FALSE;
	}
	LRESULT TinyControl::OnIniMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyHandleHMENU** pps = TinyApplication::GetInstance()->GetMapHMENU().Lookup(reinterpret_cast<HMENU>(wParam));
		if (pps != NULL)
		{
			m_pMenu = static_cast<TinyMenu*>(*pps);
		}
		return FALSE;
	}
	LRESULT TinyControl::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_POSCHANGING(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_POSCHANGED(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyControl::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	BOOL TinyControl::ShowWindow(INT nCmdShow) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ShowWindow(m_hWND, nCmdShow);
	}

	BOOL TinyControl::UpdateWindow() throw()
	{
		ASSERT(m_hWND != NULL);
		return ::UpdateWindow(m_hWND);
	}
	BOOL TinyControl::Invalidate()
	{
		return InvalidateRect(m_hWND, NULL, TRUE);
	}
	void TinyControl::CenterWindow(HWND parent, SIZE size) throw()
	{
		RECT windowBounds;
		RECT centerBounds = { 0 };
		if (parent)
		{
			::GetWindowRect(parent, &centerBounds);
		}
		else
		{
			HMONITOR monitor = MonitorFromWindow(m_hWND, MONITOR_DEFAULTTONEAREST);
			if (monitor != NULL)
			{
				MONITORINFO mi = { 0 };
				mi.cbSize = sizeof(mi);
				GetMonitorInfo(monitor, &mi);
				centerBounds = mi.rcWork;
			}
		}
		windowBounds.left = centerBounds.left + (centerBounds.right - centerBounds.left - size.cx) / 2;
		windowBounds.right = windowBounds.left + size.cx;
		windowBounds.top = centerBounds.top + (centerBounds.bottom - centerBounds.top - size.cy) / 2;
		windowBounds.bottom = windowBounds.top + size.cy;
		if (::GetWindowLong(m_hWND, GWL_STYLE) & WS_CHILD)
		{
			POINT topleft = { windowBounds.left, windowBounds.top };
			::MapWindowPoints(HWND_DESKTOP, parent, &topleft, 1);
			windowBounds.left = topleft.x;
			windowBounds.top = topleft.y;
			windowBounds.right = windowBounds.left + size.cx;
			windowBounds.bottom = windowBounds.top + size.cy;
		}
		WINDOWINFO win_info = { 0 };
		win_info.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(m_hWND, &win_info);
		if (AdjustWindowRectEx(&windowBounds, win_info.dwStyle, FALSE, win_info.dwExStyle))
		{
			SetWindowPos(m_hWND, 0, windowBounds.left, windowBounds.top,
				windowBounds.right - windowBounds.left,
				windowBounds.bottom - windowBounds.top,
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
	DWORD TinyControl::GetStyle() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)GetWindowLong(m_hWND, GWL_STYLE);
	}
	DWORD TinyControl::GetExStyle() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)GetWindowLong(m_hWND, GWL_EXSTYLE);
	}
	BOOL  TinyControl::GetWindowRect(LPRECT lprect)
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)::GetWindowRect(m_hWND, lprect);
	}
	BOOL  TinyControl::GetClientRect(LPRECT lprect)
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)::GetClientRect(m_hWND, lprect);
	}

	BOOL TinyControl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		DWORD dwStyle = ::GetWindowLong(m_hWND, GWL_STYLE);
		DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
		if (dwStyle == dwNewStyle)
			return FALSE;
		::SetWindowLong(m_hWND, GWL_STYLE, dwNewStyle);
		if (nFlags != 0)
		{
			return ::SetWindowPos(m_hWND, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
		}
		return TRUE;
	}

	BOOL TinyControl::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		DWORD dwStyle = ::GetWindowLong(m_hWND, GWL_EXSTYLE);
		DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
		if (dwStyle == dwNewStyle)
			return FALSE;
		::SetWindowLong(m_hWND, GWL_EXSTYLE, dwNewStyle);
		if (nFlags != 0)
		{
			return ::SetWindowPos(m_hWND, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
		}
		return TRUE;
	}
	BOOL TinyControl::SetSize(INT cx, INT cy) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyControl::SetPosition(INT x, INT y) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyControl::SetSize(const TinySize& size) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyControl::SetPosition(const TinyPoint& pos) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyControl::ClientToScreen(LPPOINT lpPoint) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ClientToScreen(m_hWND, lpPoint);
	}

	BOOL TinyControl::ClientToScreen(LPRECT lpRect) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		if (!::ClientToScreen(m_hWND, (LPPOINT)lpRect))
			return FALSE;
		return ::ClientToScreen(m_hWND, ((LPPOINT)lpRect) + 1);
	}

	BOOL TinyControl::ScreenToClient(LPPOINT lpPoint) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ScreenToClient(m_hWND, lpPoint);
	}

	BOOL TinyControl::ScreenToClient(LPRECT lpRect) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		if (!::ScreenToClient(m_hWND, (LPPOINT)lpRect))
			return FALSE;
		return ::ScreenToClient(m_hWND, ((LPPOINT)lpRect) + 1);
	}
	void TinyControl::SetDefaultFont()
	{
		::SendMessage(m_hWND, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
	}
	void TinyControl::SetFont(HFONT hFont, BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_SETFONT, (WPARAM)hFont, bRedraw);
	}
	HFONT TinyControl::GetFont() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HFONT)::SendMessage(m_hWND, WM_GETFONT, 0, 0);
	}
}
