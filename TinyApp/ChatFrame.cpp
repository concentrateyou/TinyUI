#include "stdafx.h"
#include "ChatFrame.h"

ChatFrame::ChatFrame()
{
}


ChatFrame::~ChatFrame()
{
}

LRESULT ChatFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL ChatFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD ChatFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD ChatFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR ChatFrame::RetrieveClassName()
{
	return TEXT("ChatFrame");
}

LPCSTR ChatFrame::RetrieveTitle()
{
	return TEXT("ChatFrame");
}

HICON ChatFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT ChatFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_rectTracker.m_rectangle = { 10,10,100,100 };
	m_rectTracker.m_handleSize = 4;
	return FALSE;
}

LRESULT ChatFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT ChatFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT ps = { 0 };

	HDC hDC = BeginPaint(m_hWND, &ps);
	TinyDC dc(hDC);
	m_rectTracker.Draw(&dc);
	dc.Detach();
	EndPaint(m_hWND, &ps);
	return FALSE;
}

LRESULT ChatFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT ChatFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT ChatFrame::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (m_rectTracker.SetCursor(m_hWND, LOWORD(lParam)))
	{
		bHandled = TRUE;
	}
	return FALSE;
}

LRESULT ChatFrame::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	INT nHandle = m_rectTracker.HitTest(pos);
	if (nHandle >= 0)
	{
		m_rectTracker.TrackRubberBand(m_hWND, pos, TRUE);
	}
	else
	{
		m_rectTracker.Track(m_hWND, pos, FALSE);
	}
	return FALSE;
}

LRESULT ChatFrame::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT ChatFrame::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//Invalidate();
	return FALSE;
}
