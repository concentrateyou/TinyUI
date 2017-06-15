#include "stdafx.h"
#include "VideoView.h"

namespace MShow
{
	VideoView::VideoView()
	{
	}

	VideoView::~VideoView()
	{
	}
	LRESULT VideoView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL VideoView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD VideoView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD VideoView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR VideoView::RetrieveClassName()
	{
		return TEXT("VideoView");
	}

	LPCSTR VideoView::RetrieveTitle()
	{
		return TEXT("VideoView");
	}

	HICON VideoView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT VideoView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VideoView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VideoView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT VideoView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VideoView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


