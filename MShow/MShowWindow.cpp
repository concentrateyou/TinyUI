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
		return TEXT("MShowWindow");
	}

	LPCSTR MShowWindow::RetrieveTitle()
	{
		return TEXT("MShowWindow");
	}

	HICON MShowWindow::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MShowWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &s, 0);
		INT cx = 818;
		INT cy = 730;
		CenterWindow(NULL, { cx, cy });
		m_previewView.Create(m_hWND, 25, 25, 768, 432);
		m_txtPreviewURL.Create(m_hWND, 25, 475, 500, 23);
		m_btnPreview.Create(m_hWND, 530, 475, 75, 23);
		m_btnPreview.SetText("打开URL");
		m_btnRecord.Create(m_hWND, 620, 475, 75, 23);
		m_btnRecord.SetText("开始录音");
		m_analyserBAR.Create(m_hWND, 25, 518, 300, 180);
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
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MShowWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
		return FALSE;
	}

	void MShowWindow::OnTabChange(void*)
	{

	}
}