#include "stdafx.h"
#include "MainFrame.h"


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
	return TEXT("CUIFrame");
}

LPCSTR CMainFrame::RetrieveTitle()
{
	return TEXT("CUIFrame");
}

HICON CMainFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT CMainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	m_onVideoStart.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnVideoStart));
	m_onVideoStop.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnVideoStop));
	m_videoStart.Create(m_hWND, 20, 50, 150, 23);
	m_videoStart.EVENT_Click += m_onVideoStart;
	m_videoStart.SetText("ø™ º ”∆µ≤∂ªÒ");
	m_videoStop.Create(m_hWND, 20, 80, 150, 23);
	m_videoStop.EVENT_Click += m_onVideoStop;
	m_videoStop.SetText("‘›Õ£ ”∆µ≤∂ªÒ");

	m_onAudioStart.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioStart));
	m_onAudioStop.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioStop));
	m_audioStart.Create(m_hWND, 400, 50, 150, 23);
	m_audioStart.EVENT_Click += m_onAudioStart;
	m_audioStart.SetText("ø™ º“Ù∆µ≤∂ªÒ");
	m_audioStop.Create(m_hWND, 400, 80, 150, 23);
	m_audioStop.EVENT_Click += m_onAudioStop;
	m_audioStop.SetText("‘›Õ£“Ù∆µ≤∂ªÒ");

	m_control.Create(m_hWND, 20, 120, 1024, 768);
	m_control.SetText("≤‚ ‘");

	m_videoDevice1.Create(m_hWND, 20, 20, 150, 23);
	m_onVideoChange1.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnVideoSelectChange1));
	m_videoDevice1.EVENT_SelectChange += m_onVideoChange1;
	m_videoDevice2.Create(m_hWND, 180, 20, 150, 23);
	m_onVideoChange2.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnVideoSelectChange2));
	m_videoDevice2.EVENT_SelectChange += m_onVideoChange2;
	m_videoNames.clear();
	DShow::VideoCapture::GetDevices(m_videoNames);
	for (UINT i = 0; i < m_videoNames.size(); i++)
	{
		m_videoDevice1.AddString(m_videoNames[i].name().c_str());
	}

	m_audioDevice1.Create(m_hWND, 400, 20, 150, 23);
	m_onAudioChange1.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioSelectChange1));
	m_audioDevice1.EVENT_SelectChange += m_onAudioChange1;
	m_audioDevice2.Create(m_hWND, 600, 20, 150, 23);
	m_onAudioChange2.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioSelectChange2));
	m_audioDevice2.EVENT_SelectChange += m_onAudioChange2;
	m_audioNames.clear();
	DShow::AudioCapture::GetDevices(m_audioNames);
	for (UINT i = 0; i < m_audioNames.size(); i++)
	{
		m_audioDevice1.AddString(m_audioNames[i].name().c_str());
	}



	return FALSE;
}

LRESULT CMainFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (m_renderTask)
	{
		m_renderTask->Close(INFINITE);
	}
	m_videoDevice.Uninitialize();
	m_audioDevice.Uninitialize();

	m_videoStart.EVENT_Click -= m_onVideoStart;
	m_videoStop.EVENT_Click -= m_onVideoStop;
	m_videoDevice1.EVENT_SelectChange -= m_onVideoChange1;
	m_videoDevice2.EVENT_SelectChange -= m_onVideoChange2;

	m_audioStart.EVENT_Click -= m_onAudioStart;
	m_audioStop.EVENT_Click -= m_onAudioStop;
	m_audioDevice1.EVENT_SelectChange -= m_onAudioChange1;
	m_audioDevice2.EVENT_SelectChange -= m_onAudioChange2;

	m_capture.Close();

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

void CMainFrame::OnVideoSelectChange1(INT index)
{
	m_videoParams.clear();
	m_videoDevice2.ResetContent();
	DShow::VideoCapture::GetDeviceParams(m_videoNames[index], m_videoParams);
	for (UINT i = 0; i < m_videoParams.size(); i++)
	{
		m_videoDevice2.AddString(m_videoParams[i].ToString().c_str());
	}
}

void CMainFrame::OnVideoSelectChange2(INT index)
{
	const DShow::VideoCaptureParam& param = m_videoParams[index];
	m_videoDevice.Uninitialize();
	m_videoDevice.Initialize(m_videoNames[m_videoDevice1.GetCurSel()]);
	m_videoDevice.Allocate(param);
	if (m_renderTask)
	{
		m_renderTask->Close(INFINITE);
	}
	m_renderTask.Reset(new RenderTask(m_control.Handle(), &m_videoDevice, param));
	m_renderTask->Submit();
}

void CMainFrame::OnAudioSelectChange1(INT index)
{
	m_audioParams.clear();
	m_audioDevice2.ResetContent();
	DShow::AudioCapture::GetDeviceParams(m_audioNames[index], m_audioParams);
	for (UINT i = 0; i < m_audioParams.size(); i++)
	{
		m_audioDevice2.AddString(m_audioParams[i].ToString().c_str());
	}
}
void CMainFrame::OnAudioSelectChange2(INT index)
{
	const DShow::AudioCaptureParam& param = m_audioParams[index];
	m_audioDevice.Uninitialize();
	m_audioDevice.Initialize(m_audioNames[m_audioDevice1.GetCurSel()]);
	m_audioDevice.Allocate(param);

	m_waveFile.Create("D:\\1234.wav", &param.GetFormat());
	m_capture.SetOutputFormat(param.GetFormat());
	m_wasCB = BindCallback(&CMainFrame::OnDataAvailable, this);
	m_capture.SetCallback(m_wasCB);
	m_capture.Open();
	m_capture.Start();
}

void CMainFrame::OnVideoStart(void*, INT)
{
	m_videoDevice.Start();
}
void CMainFrame::OnVideoStop(void*, INT)
{
	m_videoDevice.Stop();
}

void CMainFrame::OnAudioStart(void*, INT)
{
	m_audioDevice.Start();
}
void CMainFrame::OnAudioStop(void*, INT)
{
	m_audioDevice.Stop();
}

void CMainFrame::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
{
	m_waveFile.Write(bits, size);
}

