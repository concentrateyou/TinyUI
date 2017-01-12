#include "stdafx.h"
#include "MainUI.h"
#include "Resource.h"
#include "GameDlg.h"
#include "MediaCaptureDlg.h"
#include "WindowDlg.h"
#include "Snapshot.h"

namespace DXApp
{
	MainUI::MainUI()
	{
	}
	MainUI::~MainUI()
	{
	}
	BOOL MainUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy, DXWindow* pDXWND)
	{
		m_pDXWND = pDXWND;
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
		CreateUI();
		return FALSE;
	}
	LRESULT MainUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		DestoryUI();
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

	void MainUI::CreateUI()
	{
		ASSERT(m_pDXWND);
		m_onDXWLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MainUI::OnDXWLButtonDown));
		m_pDXWND->EVENT_RBUTTONDOWN += m_onDXWLButtonDown;

		m_broadcast.Create(m_hWND, 0, 0, 0, 0);
		m_broadcast.SetText("开始直播");
		m_onBroadcastClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnBroadcastClick));
		m_broadcast.EVENT_CLICK += m_onBroadcastClick;

		m_record.Create(m_hWND, 0, 0, 0, 0);
		m_record.SetText("开始录制");
		m_onRecordClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnRecordClick));
		m_record.EVENT_CLICK += m_onRecordClick;

		m_game.Create(m_hWND, 0, 0, 0, 0);
		m_game.SetText("游戏");
		m_onGameClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnGameClick));
		m_game.EVENT_CLICK += m_onGameClick;

		m_screen.Create(m_hWND, 0, 0, 0, 0);
		m_screen.SetText("屏幕");
		m_onScreenClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnScreenClick));
		m_screen.EVENT_CLICK += m_onScreenClick;

		m_window.Create(m_hWND, 0, 0, 0, 0);
		m_window.SetText("窗口");
		m_onWindowClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnWindowClick));
		m_window.EVENT_CLICK += m_onWindowClick;

		m_capture.Create(m_hWND, 0, 0, 0, 0);
		m_capture.SetText("采集");
		m_onCaptureClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnCaptureClick));
		m_capture.EVENT_CLICK += m_onCaptureClick;

		m_text.Create(m_hWND, 0, 0, 0, 0);
		m_text.SetText("文本");
		m_onTextClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnTextClick));
		m_text.EVENT_CLICK += m_onTextClick;

		m_image.Create(m_hWND, 0, 0, 0, 0);
		m_image.SetText("图片");
		m_onImageClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnImageClick));
		m_image.EVENT_CLICK += m_onImageClick;

		RECT s = { 0 };
		m_pDXWND->GetClientRect(&s);
		if (m_renderTask.Initialize(m_pDXWND, TO_CX(s), TO_CY(s), 30))
		{
			m_renderTask.Submit();
		}
	}
	void MainUI::DestoryUI()
	{
		m_pDXWND->EVENT_RBUTTONDOWN -= m_onDXWLButtonDown;

		m_renderTask.Close();

		m_broadcast.EVENT_CLICK -= m_onBroadcastClick;
		m_record.EVENT_CLICK -= m_onRecordClick;
		m_game.EVENT_CLICK -= m_onGameClick;
		m_screen.EVENT_CLICK -= m_onScreenClick;
		m_window.EVENT_CLICK -= m_onWindowClick;
		m_capture.EVENT_CLICK -= m_onCaptureClick;
		m_text.EVENT_CLICK -= m_onTextClick;
		m_image.EVENT_CLICK -= m_onImageClick;
	}
	void MainUI::OnDXWLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		m_pDXWND->ClientToScreen(&pos);
		m_pDXWND->ShowContextMenu(pos);
	}
	void MainUI::OnBroadcastClick(void*, INT)
	{

	}
	void MainUI::OnRecordClick(void*, INT)
	{

	}
	void MainUI::OnGameClick(void*, INT)
	{
		GameDlg dlg;
		if (dlg.DoModal(m_hWND, IDD_DLG_GAME) == IDOK)
		{
			HWND hWND = dlg.GetGameWND();
			CHAR className[MAX_PATH];
			::GetClassName(hWND, className, MAX_PATH);
			DWORD processID;
			GetWindowThreadProcessId(hWND, &processID);
			if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID))
			{

				DWORD size = MAX_PATH;
				CHAR windowExecutable[MAX_PATH];
				if (QueryFullProcessImageName(hProcess, 0, windowExecutable, &size))
				{
					CHAR* pzName = PathFindFileName(windowExecutable);

					if (!m_renderTask.Contain(&m_gameScene))
					{
						m_gameScene.Initialize(m_renderTask.GetGraphics()->GetDX11());
						m_gameScene.SetConfig(className, pzName, TEXT("D:\\Develop\\TinyUI\\Debug\\GameDetour.dll"));
						if (m_renderTask.Add(&m_gameScene))
						{
							m_gameScene.BeginScene();
						}
					}
					else
					{
						m_gameScene.EndScene();
						m_gameScene.Initialize(m_renderTask.GetGraphics()->GetDX11());
						m_gameScene.BeginScene();
					}
				}
				CloseHandle(hProcess);
				hProcess = NULL;
			}
		}
	}
	void MainUI::OnScreenClick(void*, INT)
	{
		m_snapshot.Create(m_hWND);
		m_onSelected.Reset(new Delegate<void(RECT)>(this, &MainUI::OnSelected));
		m_snapshot.EVENT_SELECTED += m_onSelected;
		m_snapshot.ShowWindow(SW_SHOWNORMAL);
	}
	void MainUI::OnWindowClick(void*, INT)
	{
		WindowDlg dlg;
		if (dlg.DoModal(m_hWND, IDD_DLG_WINDOW) == IDOK)
		{
			if (IsWindow(dlg.GetHWND()))
			{
				m_renderTask.Remove(&m_windowScene);
				m_windowScene.EndScene();
				m_windowScene.Initialize(m_renderTask.GetGraphics()->GetDX11(), dlg.GetHWND());
				if (m_renderTask.Add(&m_windowScene))
				{
					m_windowScene.BeginScene();
				}
			}
		}
	}
	void MainUI::OnCaptureClick(void*, INT)
	{
		MediaCaptureDlg dlg;
		if (dlg.DoModal(m_hWND, IDD_DIG_MEDIACAPTURE) == IDOK)
		{
			m_renderTask.Remove(&m_videoScene);
			m_videoScene.EndScene();
			m_videoScene.Initialize(m_renderTask.GetGraphics()->GetDX11(), *dlg.GetVideoName(), *dlg.GetVideoParam());
			if (m_renderTask.Add(&m_videoScene))
			{
				m_videoScene.BeginScene();
			}
		}
	}
	void MainUI::OnTextClick(void*, INT)
	{

	}
	void MainUI::OnImageClick(void*, INT)
	{
		TinyString filter = "Image (*.png; *.bmp; *.jpg)|*.png;*.bmp;*.jpg||";
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY, filter.STR());
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			m_renderTask.Remove(&m_imageScene);
			m_imageScene.EndScene();
			m_imageScene.Initialize(m_renderTask.GetGraphics()->GetDX11(), dlg.GetPathName());
			if (m_renderTask.Add(&m_imageScene))
			{
				m_imageScene.BeginScene();
			}
		}
	}
	void MainUI::OnSelected(RECT rect)
	{
		m_snapshot.EVENT_SELECTED -= m_onSelected;
		m_snapshot.DestroyWindow();
		m_snapshot.m_rectangle.SetRectEmpty();
		m_renderTask.Remove(&m_screenScene);
		m_screenScene.EndScene();
		m_screenScene.Initialize(m_renderTask.GetGraphics()->GetDX11(), rect);
		if (m_renderTask.Add(&m_screenScene))
		{
			m_screenScene.BeginScene();
		}
	}

	void MainUI::Resize(INT cx, INT cy)
	{
		INT offsetX = 15;
		INT offsetY = 10;
		m_broadcast.SetPosition(offsetX, offsetY);
		m_broadcast.SetSize(100, 30);
		m_record.SetPosition(offsetX + 100, offsetY);
		m_record.SetSize(100, 30);

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
}