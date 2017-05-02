#include "stdafx.h"
#include "DXFrameUI.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#define FRAME_RATE 30

namespace DXApp
{
	IMPLEMENT_DYNAMIC(DXFrameUI, TinyControl);
	DXFrameUI::DXFrameUI()
	{
	}
	DXFrameUI::~DXFrameUI()
	{
	}
	BOOL DXFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD DXFrameUI::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}
	DWORD DXFrameUI::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}
	LPCSTR DXFrameUI::RetrieveClassName()
	{
		return TEXT("IQiyiWindowClass");
	}
	LPCSTR DXFrameUI::RetrieveTitle()
	{
		return TEXT("DXFrameUI");
	}
	HICON DXFrameUI::RetrieveIcon()
	{
		return NULL;
	}

	void DXFrameUI::Resize(INT cx, INT cy)
	{
		m_window.SetPosition(0, 0);
		m_window.SetSize(cx, 720);
		m_mainUI.SetPosition(0, 720);
		m_mainUI.SetSize(cx, cy - 720);
	}

	LRESULT DXFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		CreateUI();

		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		INT cx = 1280;
		INT cy = TO_CY(rect) * 5 / 6;
		CenterWindow(NULL, { cx, cy });

		return FALSE;
	}
	LRESULT DXFrameUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT DXFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		PostQuitMessage(0);
		return FALSE;
	}
	LRESULT DXFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT DXFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}

	LRESULT DXFrameUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
		Resize(cx, cy);
		return FALSE;
	}

	BOOL DXFrameUI::CreateUI()
	{
		RECT s = { 0 };
		GetClientRect(&s);
		BOOL bRes = m_window.Create(m_hWND, 0, 0, TO_CX(s), TO_CY(s) * 3 / 4);
		if (!bRes)
			return FALSE;
		bRes = m_mainUI.Create(m_hWND, 10, TO_CY(s) * 3 / 4 + 15, TO_CX(s), TO_CY(s) / 4, &m_window);
		if (!bRes)
			return FALSE;
		return TRUE;
	}
}