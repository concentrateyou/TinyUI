#include "stdafx.h"
#include <string>
#include "MainUI.h"
#include "GameDlg.h"
#include "MediaCaptureDlg.h"
#include "WindowDlg.h"
#include "TextDlg.h"
#include "Snapshot.h"
#include "Resource.h"
#include "Media/TinyWASAPIAudio.h"
using namespace TinyUI::Media;
using namespace std;

namespace DXApp
{
	IMPLEMENT_DYNAMIC(MainUI, TinyControl);
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

		m_broadcast.Create(m_hWND, 0, 0, 0, 0);
		m_broadcast.SetText("开始直播");
		m_onBroadcastClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnBroadcastClick));
		m_broadcast.EVENT_CLICK += m_onBroadcastClick;

		m_record.Create(m_hWND, 0, 0, 0, 0);
		m_record.SetText("开始录制");
		m_onRecordClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnRecordClick));
		m_record.EVENT_CLICK += m_onRecordClick;

		m_allowAero.Create(m_hWND, 0, 0, 0, 0);
		m_allowAero.SetText("禁用Aero");
		m_onCheckAeroClick.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnAeroClick));
		m_allowAero.EVENT_CLICK += m_onCheckAeroClick;

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

		m_lblSpeaker.Create(m_hWND, 0, 0, 100, 30);
		m_lblSpeaker.ModifyStyle(SS_BITMAP, SS_LEFT);
		m_lblSpeaker.SetText("扬声器:");
		m_lblSpeaker.SetDefaultFont();

		m_lblMicrophone.Create(m_hWND, 0, 0, 100, 30);
		m_lblMicrophone.ModifyStyle(SS_BITMAP, SS_LEFT);
		m_lblMicrophone.SetText("麦克风:");
		m_lblMicrophone.SetDefaultFont();

		m_lbl1.Create(m_hWND, 0, 0, 100, 25);
		m_lbl1.ModifyStyle(SS_BITMAP, SS_LEFT);
		m_lbl1.SetText("设置RTMP地址:");
		m_lbl1.SetDefaultFont();
		m_txtRtmpURL.Create(m_hWND, 0, 0, 300, 21);
		m_txtRtmpURL.SetText("rtmp://10.121.86.127/live/test");

		m_lbl2.Create(m_hWND, 0, 0, 100, 25);
		m_lbl2.ModifyStyle(SS_BITMAP, SS_LEFT);
		m_lbl2.SetText("设置推流分辨率:");
		m_lbl2.SetDefaultFont();
		m_resolution.Create(m_hWND, 0, 0, 200, 21);
		m_resolution.AddString("640*480");
		m_resolution.AddString("1280*720");
		m_resolution.AddString("1600*900");

		m_speaker.Create(m_hWND, 0, 0, 0, 0);
		m_microphone.Create(m_hWND, 0, 0, 0, 0);
		m_onMicrophoneVolumeChange.Reset(new Delegate<void(void*, INT)>(this, &MainUI::OnMicrophoneVolumeChange));
		m_microphone.EVENT_POSCHANGING += m_onMicrophoneVolumeChange;

		RECT s = { 0 };
		m_pDXWND->GetClientRect(&s);
		if (m_renderTask.Initialize(m_pDXWND, TO_CX(s), TO_CY(s), 50))
		{
			m_renderTask.Submit();
		}
	}
	void MainUI::DestoryUI()
	{
		m_renderTask.Close();
		if (m_videoEncode != NULL)
			m_videoEncode->Close();
		if (m_audioEncode != NULL)
			m_audioEncode->Close();
		if (m_publishTask != NULL)
			m_publishTask->Close();

		m_broadcast.EVENT_CLICK -= m_onBroadcastClick;
		m_record.EVENT_CLICK -= m_onRecordClick;
		m_allowAero.EVENT_CLICK -= m_onCheckAeroClick;
		m_game.EVENT_CLICK -= m_onGameClick;
		m_screen.EVENT_CLICK -= m_onScreenClick;
		m_window.EVENT_CLICK -= m_onWindowClick;
		m_capture.EVENT_CLICK -= m_onCaptureClick;
		m_text.EVENT_CLICK -= m_onTextClick;
		m_image.EVENT_CLICK -= m_onImageClick;
		m_microphone.EVENT_POSCHANGING -= m_onMicrophoneVolumeChange;
	}
	void MainUI::OnBroadcastClick(void*, INT)
	{
		INT index = m_resolution.GetCurSel();
		TinySize scale;
		switch (index)
		{
		case 0:
			scale.SetSize(640, 480);
			break;
		case 1:
			scale.SetSize(1280, 720);
			break;
		case 2:
			scale.SetSize(1600, 900);
			break;
		}
		m_renderTask.SetPulgSize(XMFLOAT2(scale.cx, scale.cy));
		if (m_videoEncode != NULL)
			m_videoEncode->Close();
		m_videoEncode.Reset(new VideoEncode(&m_renderTask));
		m_videoEncode->Initialize(scale, 30, 1000);
		if (m_audioEncode != NULL)
			m_audioEncode->Close();
		m_audioEncode.Reset(new AudioEncode(m_audioName, m_audioParam));
		m_audioEncode->Open(128);
		if (m_publishTask != NULL)
			m_publishTask->Close(INFINITE);
		m_publishTask.Reset(new PublishTask(m_audioEncode, m_videoEncode));
		TinyString rtmpURL;
		rtmpURL.Resize(256);
		m_txtRtmpURL.GetText(rtmpURL.STR(), 256);
		m_publishTask->Connect(rtmpURL);
		m_publishTask->Submit();
		m_videoEncode->Submit();
		m_audioEncode->Submit();
	}
	void MainUI::OnRecordClick(void*, INT)
	{

	}
	void MainUI::OnAeroClick(void*, INT)
	{
		BOOL bComposition = FALSE;
		DwmIsCompositionEnabled(&bComposition);
		DwmEnableComposition(!bComposition);
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
					string str;
					str.resize(MAX_PATH);
					GetModuleFileName(NULL, &str[0], MAX_PATH);
					str = str.substr(0, str.find_last_of("\\", string::npos, 1));
					str += "\\GameDetour.dll";
					ASSERT(PathFileExists(str.c_str()));
					if (!m_renderTask.Contain(&m_gameScene))
					{
						m_gameScene.Initialize(m_renderTask.GetGraphics().GetDX11());
						m_gameScene.SetConfig(className, pzName, str.c_str());
						if (m_renderTask.Add(&m_gameScene))
						{
							m_gameScene.Allocate(m_renderTask.GetGraphics().GetDX11());
						}
					}
					else
					{
						m_gameScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
						m_gameScene.Initialize(m_renderTask.GetGraphics().GetDX11());
						m_gameScene.SetConfig(className, pzName, str.c_str());
						m_gameScene.Allocate(m_renderTask.GetGraphics().GetDX11());
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
				m_windowScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
				m_windowScene.Initialize(m_renderTask.GetGraphics().GetDX11(), dlg.GetHWND());
				if (m_renderTask.Add(&m_windowScene))
				{
					m_windowScene.Allocate(m_renderTask.GetGraphics().GetDX11());
				}
			}
		}
	}
	void MainUI::OnCaptureClick(void*, INT)
	{
		MediaCaptureDlg dlg;
		if (dlg.DoModal(m_hWND, IDD_DLG_MEDIACAPTURE) == IDOK)
		{
			if (dlg.GetAudioName() != NULL)
			{
				m_audioName = *dlg.GetAudioName();
				m_audioParam = *dlg.GetAudioParam();
			}
			if (dlg.GetVideoName() != NULL && dlg.GetVideoParam() != NULL)
			{
				m_renderTask.Remove(&m_videoScene);
				m_videoScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
				m_videoScene.Initialize(m_renderTask.GetGraphics().GetDX11(), *dlg.GetVideoName(), *dlg.GetVideoParam());
				if (m_renderTask.Add(&m_videoScene))
				{
					m_videoScene.Allocate(m_renderTask.GetGraphics().GetDX11());
				}
			}
		}
	}
	void MainUI::OnTextClick(void*, INT)
	{
		m_onTextChange.Reset(new Delegate<void(void*)>(this, &MainUI::OnContextChange));
		TextDlg dlg;
		dlg.EVENT_CONTEXTCHANGE += m_onTextChange;
		if (dlg.DoModal(m_hWND, IDD_DLG_TEXT) == IDOK)
		{
			m_renderTask.Remove(&m_textScene);
			m_textScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
			m_textScene.Initialize(m_renderTask.GetGraphics().GetDX11(), dlg.GetText(), dlg.GetFormat(), dlg.GetBkColor());
			if (m_renderTask.Add(&m_textScene))
			{
				m_textScene.Allocate(m_renderTask.GetGraphics().GetDX11());
			}
		}
		dlg.EVENT_CONTEXTCHANGE -= m_onTextChange;
	}
	void MainUI::OnImageClick(void*, INT)
	{
		LPCTSTR lpszFilter = _T("Image Files(*.bmp, *.jpg, *.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			m_renderTask.Remove(&m_imageScene);
			m_imageScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
			m_imageScene.Initialize(m_renderTask.GetGraphics().GetDX11(), dlg.GetPathName());
			if (m_renderTask.Add(&m_imageScene))
			{
				m_imageScene.Allocate(m_renderTask.GetGraphics().GetDX11());
			}
		}
	}
	void MainUI::OnAudioClick(void*, INT)
	{

	}
	void MainUI::OnSelected(RECT rect)
	{
		TinyRectangle s = rect;
		s.NormalizeRect();
		m_snapshot.EVENT_SELECTED -= m_onSelected;
		m_snapshot.DestroyWindow();
		m_snapshot.SetEmpty();
		m_renderTask.Remove(&m_screenScene);
		m_screenScene.Deallocate(m_renderTask.GetGraphics().GetDX11());
		m_screenScene.Initialize(m_renderTask.GetGraphics().GetDX11(), s);
		if (m_renderTask.Add(&m_screenScene))
		{
			m_screenScene.Allocate(m_renderTask.GetGraphics().GetDX11());
		}
	}
	void MainUI::OnContextChange(void* sender)
	{
		TextDlg* dlg = reinterpret_cast<TextDlg*>(sender);
		if (dlg != NULL)
		{


		}
	}
	void MainUI::OnMicrophoneVolumeChange(void*, INT pos)
	{
		if (m_audioEncode)
		{
			m_audioEncode->GetCapture().SetVolume(pos);
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

		m_allowAero.SetPosition(offsetX + 250, offsetY);
		m_allowAero.SetSize(100, 30);

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

		m_lblSpeaker.SetPosition(offsetX + 100 * 6, offsetY + 15);
		m_lblSpeaker.SetSize(100, 30);

		m_lblMicrophone.SetPosition(offsetX + 100 * 8, offsetY + 15);
		m_lblMicrophone.SetSize(100, 30);

		m_speaker.SetPosition(offsetX + 100 * 6, offsetY + 50);
		m_speaker.SetSize(200, 30);
		m_speaker.SetRange(0, 100);

		m_microphone.SetPosition(offsetX + 100 * 8, offsetY + 50);
		m_microphone.SetSize(200, 30);
		m_microphone.SetRange(0, 100);

		m_lbl1.SetPosition(offsetX + 100 * 0, offsetY + 103);
		m_txtRtmpURL.SetPosition(offsetX + 100 * 1, offsetY + 100);

		m_lbl2.SetPosition(offsetX + 100 * 4 + 20, offsetY + 103);
		m_resolution.SetPosition(offsetX + 100 * 5 + 20, offsetY + 100);
	}
}