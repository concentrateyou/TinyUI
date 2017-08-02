#include "stdafx.h"
#include <math.h>
#include "MAudioView.h"
#include "resource.h"

namespace MShow
{
	MAudioView::MAudioView()
	{
	}

	MAudioView::~MAudioView()
	{
	}

	BOOL MAudioView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MAudioView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MAudioView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MAudioView::RetrieveClassName()
	{
		return TEXT("MAudioView");
	}

	LPCSTR MAudioView::RetrieveTitle()
	{
		return TEXT("MAudioView");
	}

	HICON MAudioView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MAudioView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MAudioView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


