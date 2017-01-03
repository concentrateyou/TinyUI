#include "stdafx.h"
#include "MainUI.h"

MainUI::MainUI()
{
}
MainUI::~MainUI()
{
}
BOOL MainUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD MainUI::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | WS_BORDER);
}
DWORD MainUI::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}
LPCSTR MainUI::RetrieveClassName()
{
	return TEXT("MainUI");
}
LPCSTR MainUI::RetrieveTitle()
{
	return TEXT("MainUI");
}
HICON MainUI::RetrieveIcon()
{
	return NULL;
}

LRESULT MainUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	BuildUI();
	return FALSE;
}
LRESULT MainUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return FALSE;
}
LRESULT MainUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT MainUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT MainUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT MainUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	INT cx = LOWORD(lParam);
	INT cy = HIWORD(lParam);
	Resize(cx, cy);
	return FALSE;
}

void MainUI::BuildUI()
{
	m_broadcast.Create(m_hWND, 0, 0, 0, 0);
	m_broadcast.SetText("开始直播");

	m_recording.Create(m_hWND, 0, 0, 0, 0);
	m_recording.SetText("开始录制");

	m_game.Create(m_hWND, 0, 0, 0, 0);
	m_game.SetText("游戏");

	m_screen.Create(m_hWND, 0, 0, 0, 0);
	m_screen.SetText("屏幕");

	m_window.Create(m_hWND, 0, 0, 0, 0);
	m_window.SetText("窗口");

	m_capture.Create(m_hWND, 0, 0, 0, 0);
	m_capture.SetText("采集");

	m_text.Create(m_hWND, 0, 0, 0, 0);
	m_text.SetText("文本");

	m_image.Create(m_hWND, 0, 0, 0, 0);
	m_image.SetText("图片");

}

void MainUI::Resize(INT cx, INT cy)
{
	INT offsetX = 15;
	INT offsetY = 10;
	m_broadcast.SetPosition(offsetX, offsetY);
	m_broadcast.SetSize(100, 30);
	m_recording.SetPosition(offsetX + 100, offsetY);
	m_recording.SetSize(100, 30);

	m_game.SetPosition(offsetX + 100 * 0, offsetY + 50);
	m_game.SetSize(100, 30);

	m_screen.SetPosition(offsetX + 100 * 1, offsetY + 50);
	m_screen.SetSize(100, 30);

	m_window.SetPosition(offsetX + 100 * 2, offsetY + 50);
	m_window.SetSize(100, 30);

	m_capture.SetPosition(offsetX + 100 * 3, offsetY + 50);
	m_capture.SetSize(100, 30);

	m_text.SetPosition(offsetX + 100 * 4, offsetY + 50);
	m_text.SetSize(100, 30);

	m_image.SetPosition(offsetX + 100 * 5, offsetY + 50);
	m_image.SetSize(100, 30);
}