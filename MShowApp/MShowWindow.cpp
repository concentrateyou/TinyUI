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
		INT cx = 1300;
		INT cy = 730;
		CenterWindow(NULL, { cx, cy });
		m_tab.Create(m_hWND, 0, 440, 1300, 250);
		m_tab.GetClientRect(&s);
		s.top += 23;
		s.bottom -= 3;
		s.left += 3;
		s.right -= 3;
		m_tab.InsertItem(0, "   视频源   ");
		m_tabViews[0].Create(m_tab.Handle(), s.left, s.top, TO_CX(s), TO_CY(s));
		m_tabViews[0].ShowWindow(SW_SHOW);
		m_tab.InsertItem(1, "   图片源   ");
		m_tabViews[1].Create(m_tab.Handle(), s.left, s.top, TO_CX(s), TO_CY(s));
		m_tabViews[1].ShowWindow(SW_HIDE);
		m_tab.SetCurSel(0);
		m_onTabChange.Reset(new Delegate<void(void*)>(this, &MShowWindow::OnTabChange));
		m_tab.EVENT_SELCHANGE += m_onTabChange;
		m_previewView.Create(m_hWND, 33, 90, 512, 288);
		m_shadowView.Create(m_hWND, 33, 90, 512, 288);
		m_shadowView.ShowWindow(SW_HIDE);
		m_playView.Create(m_hWND, 756, 90, 512, 288);
		m_playVolumeView.Create(m_hWND, 756, 400, 192, 20);
		m_toggle.Create(m_hWND, 570, 175, 165, 100);
		m_toggle.SetText("切换");
		m_pusher.Create(m_hWND, 1277 - 120, 383, 100, 25);
		m_pusher.SetText("开始直播");
		m_layout[0].Create(m_hWND, 545 - 32 * 3 - 30, 378 + 5, 32, 32);
		m_layout[0].SetText("1*1");
		m_layout[1].Create(m_hWND, 545 - 32 * 2 - 20, 378 + 5, 32, 32);
		m_layout[1].SetText("2*2");
		m_layout[2].Create(m_hWND, 545 - 32 * 1 - 10, 378 + 5, 32, 32);
		m_layout[2].SetText("3*2");
		INT offset = 20;
		for (INT i = 0;i < 6;i++)
		{
			m_videoViews[i].Create(m_tabViews[0], offset, 45, 192, 108);
			m_volumeViews[i].Create(m_tabViews[0], offset, 10, 192, 20);
			m_imageViews[i].Create(m_tabViews[1], offset, 45, 192, 108);
			offset += 212;
		}

		return FALSE;
	}
	LRESULT MShowWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_tab.EVENT_SELCHANGE -= m_onTabChange;
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
		switch (m_tab.GetCurSel())
		{
		case 0:
			m_tabViews[0].ShowWindow(SW_SHOW);
			m_tabViews[1].ShowWindow(SW_HIDE);
			break;
		case 1:
			m_tabViews[1].ShowWindow(SW_SHOW);
			m_tabViews[0].ShowWindow(SW_HIDE);
			break;
		default:
			break;
		}
	}
}