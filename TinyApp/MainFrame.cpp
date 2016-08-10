#include "stdafx.h"
#include "MainFrame.h"
#include "VideoCapture.h"
using namespace Media;

CMainFrame::CMainFrame()
{

}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL CMainFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD CMainFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD CMainFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR CMainFrame::RetrieveClassName()
{
	return TEXT("IQiyiWindowClass");
}

LPCSTR CMainFrame::RetrieveTitle()
{
	return TEXT("FramwUI");
}

HICON CMainFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT CMainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	vector<VideoCapture::Name> names;
	VideoCapture::GetDevices(names);
	vector<VideoCaptureParam> params;
	VideoCapture::GetDeviceParams(names[0], params);



	/*m_inject.Create(m_hWND, 10, 10, 100, 25);
	m_inject.SetText("注入到War3");
	m_onInjectClick.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnInjectLibrary));
	m_inject.Click += m_onInjectClick;
	m_lblState.Create(m_hWND, 130, 16, 100, 25);
	m_lblState.SetText("状态");
	memset(&m_cf, 0, sizeof(m_cf));
	m_cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_EFFECTS;
	m_cf.dwEffects |= CFE_BOLD;
	m_cf.sSpacing = 5;
	m_cf.cbSize = sizeof(CHARFORMAT2);
	m_cf.crTextColor = RGB(0, 114, 193);
	m_cf.yHeight = 180;
	strcpy(m_cf.szFaceName, _T("微软雅黑"));
	m_drawCtrl.Create(m_hWND, 10, 40, 400, 400);*/
	return FALSE;
}

LRESULT CMainFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_inject.Click -= m_onInjectClick;
	return FALSE;
}

LRESULT CMainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT CMainFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void CMainFrame::OnInjectLibrary(void*, INT)
{
	RECT rect;
	::GetClientRect(m_drawCtrl, &rect);
	/*if (m_videoCapture.Initialize(m_drawCtrl, TO_CX(rect), TO_CY(rect)))
	{
		m_lblState.SetText("初始化成功!");
	}
	else
	{
		m_lblState.SetText("初始化失败!");
	}*/
}

