#include "stdafx.h"
#include <math.h>
#include "MImageView.h"
#include "resource.h"

namespace MShow
{
	MImageView::MImageView()
	{
	}

	MImageView::~MImageView()
	{
	}

	BOOL MImageView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MImageView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MImageView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MImageView::RetrieveClassName()
	{
		return TEXT("MImageView");
	}

	LPCSTR MImageView::RetrieveTitle()
	{
		return TEXT("MImageView");
	}

	HICON MImageView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MImageView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MImageView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


