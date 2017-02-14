#include "stdafx.h"
#include "FLVFrame.h"


FLVFrame::FLVFrame()
{
}


FLVFrame::~FLVFrame()
{
}

LRESULT FLVFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL FLVFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD FLVFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD FLVFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR FLVFrame::RetrieveClassName()
{
	return TEXT("FLVFrame");
}

LPCSTR FLVFrame::RetrieveTitle()
{
	return TEXT("FLVFrame");
}

HICON FLVFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT FLVFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_task.Reset(new FLVTask(m_hWND));
	m_task->Submit();
	return FALSE;
}

LRESULT FLVFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT FLVFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT s = { 0 };
	HDC hDC = BeginPaint(m_hWND, &s);

	EndPaint(m_hWND, &s);
	return FALSE;
}

LRESULT FLVFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}