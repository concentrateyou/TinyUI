#include "stdafx.h"
#include "MShowWindow.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MShowWindow, TinyControl);
	MShowWindow::MShowWindow()
	{
	}
	MShowWindow::~MShowWindow()
	{
	}
	BOOL MShowWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MShowWindow::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}
	DWORD MShowWindow::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}
	LPCSTR MShowWindow::RetrieveClassName()
	{
		return TEXT("MShowFrameUI");
	}
	LPCSTR MShowWindow::RetrieveTitle()
	{
		return TEXT("MShowFrameUI");
	}
	HICON MShowWindow::RetrieveIcon()
	{
		return NULL;
	}

	void MShowWindow::Resize(INT cx, INT cy)
	{

	}

	LRESULT MShowWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		INT cx = 1024;
		INT cy = TO_CY(rect) * 3 / 4;
		CenterWindow(NULL, { cx, cy });
		return FALSE;
	}
	LRESULT MShowWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT MShowWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}
	LRESULT MShowWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT MShowWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}

	LRESULT MShowWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
		Resize(cx, cy);
		return FALSE;
	}
}

