#include "stdafx.h"
#include "MShowWindow.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MShowWindow, TinyControl);
	MShowWindow::MShowWindow()
		:m_controller(m_previewView)
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

	MPreviewView& MShowWindow::GetPreivewView()
	{
		return m_previewView;
	}

	LRESULT MShowWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &s, 0);
		INT cx = 1300;
		INT cy = 730;
		CenterWindow(NULL, { cx, cy });
		m_previewView.Create(m_hWND, 33, 90, 512, 288);
		m_controller.Initialize(TinySize(1280, 720));
		m_tab.Create(m_hWND, 0, 440, 1300, 250);
		m_tab.GetClientRect(&s);
		s.top += 23;
		s.bottom -= 3;
		s.left += 3;
		s.right -= 3;
		m_tab.InsertItem(0, "   ��ƵԴ   ");
		m_tabViews[0].Create(m_tab.Handle(), s.left, s.top, TO_CX(s), TO_CY(s));
		m_tabViews[0].ShowWindow(SW_SHOW);
		m_tab.InsertItem(1, "   ͼƬԴ   ");
		m_tabViews[1].Create(m_tab.Handle(), s.left, s.top, TO_CX(s), TO_CY(s));
		m_tabViews[1].ShowWindow(SW_HIDE);
		m_tab.SetCurSel(0);
		m_onTabChange.Reset(new Delegate<void(void*)>(this, &MShowWindow::OnTabChange));
		m_tab.EVENT_SELCHANGE += m_onTabChange;
		INT offset = 20;
		for (INT i = 0;i < 6;i++)
		{
			VideoView* pVideoView = new VideoView(m_controller);
			pVideoView->Create(m_tabViews[0], offset, 45, 192, 108);
			m_videoViews.Add(pVideoView);
			m_volumeViews[i].Create(m_tabViews[0], offset, 10, 192, 20);
			ImageView* pImageView = new ImageView(m_controller);
			pImageView->Create(m_tabViews[1], offset, 45, 192, 108);
			m_imageViews.Add(pImageView);
			offset += 212;
		}
		return FALSE;
	}
	LRESULT MShowWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		for (INT i = 0;i < 6;i++)
		{
			m_imageViews[i]->DestroyWindow();
			SAFE_DELETE(m_imageViews[i]);
			m_videoViews[i]->DestroyWindow();
			SAFE_DELETE(m_videoViews[i]);
		}
		m_videoViews.RemoveAll();
		m_imageViews.RemoveAll();
		m_tab.EVENT_SELCHANGE -= m_onTabChange;
		m_controller.Uninitialize();
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
		//TODO
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

