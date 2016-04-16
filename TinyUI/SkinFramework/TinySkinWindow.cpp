#include "../stdafx.h"
#include "TinySkinWindow.h"

namespace TinyUI
{
	TinySkinWindow::TinySkinWindow()
	{

	}

	TinySkinWindow::~TinySkinWindow()
	{
	}

	LRESULT TinySkinWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);//退出应用程序
		return FALSE;
	}

	BOOL TinySkinWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	LRESULT TinySkinWindow::OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinySkinWindow::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		if (static_cast<BOOL>(wParam))
		{
			NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			ps->rgrc[0].left = ps->lppos->x;
			ps->rgrc[0].top = ps->lppos->y;
			ps->rgrc[0].bottom = ps->lppos->y + ps->lppos->cy;
			ps->rgrc[0].right = ps->lppos->x + ps->lppos->cx;
		}
		return FALSE;
	}
	LRESULT TinySkinWindow::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		return FALSE;
	}
	DWORD TinySkinWindow::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD TinySkinWindow::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR TinySkinWindow::RetrieveClassName()
	{
		return TEXT("FramwUI");
	}

	LPCSTR TinySkinWindow::RetrieveTitle()
	{
		return TEXT("FramwUI");
	}

	HICON TinySkinWindow::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT TinySkinWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;


		return TRUE;
	}

	LRESULT TinySkinWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		return FALSE;
	}

	LRESULT TinySkinWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinySkinWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT TinySkinWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}

	INT TinySkinWindow::GetCaptionCY()
	{
		if ((this->GetStyle() & WS_CAPTION) == 0)
		{
			return 0;
		}
		if (this->GetExStyle() & WS_EX_TOOLWINDOW)
		{
			return ::GetSystemMetrics(SM_CYSMCAPTION);
		}
		return ::GetSystemMetrics(SM_CYCAPTION);
	}
}

