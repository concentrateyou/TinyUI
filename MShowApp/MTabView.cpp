#include "stdafx.h"
#include "MTabView.h"

namespace MShow
{
	MTabView::MTabView()
	{
	}

	MTabView::~MTabView()
	{
	}
	LRESULT MTabView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL MTabView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MTabView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MTabView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MTabView::RetrieveClassName()
	{
		return TEXT("TabView");
	}

	LPCSTR MTabView::RetrieveTitle()
	{
		return TEXT("TabView");
	}

	HICON MTabView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MTabView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MTabView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MTabView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MTabView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MTabView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


