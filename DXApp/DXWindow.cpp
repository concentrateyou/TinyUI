#include "stdafx.h"
#include "DXWindow.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#define FRAME_RATE				30

namespace DXApp
{
	IMPLEMENT_DYNAMIC(DXWindow, TinyControl);
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
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
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
	LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT DXWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT DXWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
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
}