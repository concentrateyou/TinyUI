#include "stdafx.h"
#include "UIFrame.h"


CUIFrame::CUIFrame()
{

}

CUIFrame::~CUIFrame()
{
}

LRESULT CUIFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL CUIFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD CUIFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD CUIFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR CUIFrame::RetrieveClassName()
{
	return TEXT("CUIFrame");
}

LPCSTR CUIFrame::RetrieveTitle()
{
	return TEXT("CUIFrame");
}

HICON CUIFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT CUIFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_onStart.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnStart));
	m_onStop.Reset(new Delegate<void(void*, INT)>(this, &CUIFrame::OnStop));
	m_start.Create(m_hWND, 20, 50, 150, 23);
	m_start.EVENT_Click += m_onStart;
	m_start.SetText("开始捕获");
	m_stop.Create(m_hWND, 20, 80, 150, 23);
	m_stop.EVENT_Click += m_onStop;
	m_stop.SetText("暂停捕获");

	m_device1.Create(m_hWND, 20, 20, 150, 23);
	m_onChange1.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnSelectChange1));
	m_device1.EVENT_SelectChange += m_onChange1;
	m_device2.Create(m_hWND, 180, 20, 150, 23);
	m_onChange2.Reset(new Delegate<void(INT)>(this, &CUIFrame::OnSelectChange2));
	m_device2.EVENT_SelectChange += m_onChange2;
	m_names.clear();
	Media::VideoCapture::GetDevices(m_names);
	for (INT i = 0; i < m_names.size(); i++)
	{
		m_device1.AddString(m_names[i].name().c_str());
	}
	/*vector<Media::VideoCapture::Name> names;
	Media::VideoCapture::GetDevices(names);
	vector<Media::VideoCaptureParam> params;
	Media::VideoCapture::GetDeviceParams(names[0], params);
	m_device.Uninitialize();
	m_device.Initialize(names[0]);
	m_device.Allocate(params[0]);*/
	return FALSE;
}

LRESULT CUIFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT CUIFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT CUIFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void CUIFrame::OnSelectChange1(INT index)
{
	m_params.clear();
	m_device2.ResetContent();
	Media::VideoCapture::GetDeviceParams(m_names[index], m_params);
	for (INT i = 0; i < m_params.size(); i++)
	{
		m_device2.AddString(m_params[i].ToString().c_str());
	}
}

void CUIFrame::OnSelectChange2(INT index)
{
	const Media::VideoCaptureParam& param = m_params[index];
	m_device.Uninitialize();
	m_device.Initialize(m_names[m_device1.GetCurSel()]);
	m_device.Allocate(param);
}

void CUIFrame::OnStart(void*, INT)
{
	m_device.Start();
}
void CUIFrame::OnStop(void*, INT)
{
	m_device.Stop();
}

