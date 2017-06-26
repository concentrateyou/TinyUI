#include "stdafx.h"
#include <math.h>
#include "MVideoView.h"
#include "resource.h"

namespace MShow
{
	MVideoView::MVideoView(DWORD dwIndex)
		:m_dwIndex(dwIndex)
	{
	}

	MVideoView::~MVideoView()
	{
	}

	BOOL MVideoView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MVideoView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MVideoView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MVideoView::RetrieveClassName()
	{
		return TEXT("MVideoView");
	}

	LPCSTR MVideoView::RetrieveTitle()
	{
		return TEXT("MVideoView");
	}

	HICON MVideoView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MVideoView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MVideoView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


