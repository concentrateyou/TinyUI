#include "stdafx.h"
#include "DXWindow.h"

#define WINDOW_CLOSE_EVENT      TEXT("WindowClose")
#define RENDER_FINISH_EVENT     TEXT("RenderFinish")
#define PUBLISH_FINISH_EVENT    TEXT("PublishFinish")

DXWindow::DXWindow()
{
}
DXWindow::~DXWindow()
{
}
BOOL DXWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD DXWindow::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}
DWORD DXWindow::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}
LPCSTR DXWindow::RetrieveClassName()
{
	return TEXT("IQiyiWindowClass");
}
LPCSTR DXWindow::RetrieveTitle()
{
	return TEXT("DXWindow");
}
HICON DXWindow::RetrieveIcon()
{
	return NULL;
}

BOOL DXWindow::BuildEvents()
{
	if (!m_close.OpenEvent(EVENT_ALL_ACCESS, FALSE, WINDOW_CLOSE_EVENT) &&
		!m_close.CreateEvent(FALSE, FALSE, WINDOW_CLOSE_EVENT))
	{
		return FALSE;
	}
	if (!m_render.OpenEvent(EVENT_ALL_ACCESS, FALSE, RENDER_FINISH_EVENT) &&
		!m_render.CreateEvent(FALSE, FALSE, RENDER_FINISH_EVENT))
	{
		return FALSE;
	}
	if (!m_publish.OpenEvent(EVENT_ALL_ACCESS, FALSE, PUBLISH_FINISH_EVENT) &&
		!m_publish.CreateEvent(FALSE, FALSE, PUBLISH_FINISH_EVENT))
	{
		return FALSE;
	}
	return TRUE;
}

LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	BuildEvents();
	CenterWindow(NULL, { 800, 600 });
	m_graphics.Initialize(m_hWND, 800, 600);
	return FALSE;
}
LRESULT DXWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_publish.SetEvent();
	m_render.SetEvent();
	m_close.SetEvent();
	return FALSE;
}
LRESULT DXWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}
LRESULT DXWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return FALSE;
}