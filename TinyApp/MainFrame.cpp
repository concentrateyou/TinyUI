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

	m_onAudioInputStart.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioInputStart));
	m_onAudioInputStop.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioInputStop));
	m_audioInputStart.Create(m_hWND, 400, 50, 150, 23);
	m_audioInputStart.EVENT_Click += m_onAudioInputStart;
	m_audioInputStart.SetText("ø™ º“Ù∆µ≤∂ªÒ");
	m_audioInputStop.Create(m_hWND, 400, 80, 150, 23);
	m_audioInputStop.EVENT_Click += m_onAudioInputStop;
	m_audioInputStop.SetText("‘›Õ£“Ù∆µ≤∂ªÒ");

	m_onAudioOutputStart.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioOutputStart));
	m_onAudioOutputStop.Reset(new Delegate<void(void*, INT)>(this, &CMainFrame::OnAudioOutputStop));
	m_audioOutputStart.Create(m_hWND, 800, 50, 150, 23);
	m_audioOutputStart.EVENT_Click += m_onAudioOutputStart;
	m_audioOutputStart.SetText("ø™ º“Ù∆µ≤∂ªÒ");
	m_audioOutputStop.Create(m_hWND, 800, 80, 150, 23);
	m_audioOutputStop.EVENT_Click += m_onAudioOutputStop;
	m_audioOutputStop.SetText("‘›Õ£“Ù∆µ≤∂ªÒ");

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

	m_audioInput1.Create(m_hWND, 400, 20, 150, 23);
	m_onAudioInputChange1.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioInputSelectChange1));
	m_audioInput1.EVENT_SelectChange += m_onAudioInputChange1;
	m_audioInput2.Create(m_hWND, 600, 20, 150, 23);
	m_onAudioInputChange2.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioInputSelectChange2));
	m_audioInput2.EVENT_SelectChange += m_onAudioInputChange2;
	m_audioInputNames.clear();
	DShow::AudioCapture::GetDevices(m_audioInputNames);
	for (UINT i = 0; i < m_audioInputNames.size(); i++)
	{
		m_audioInput1.AddString(m_audioInputNames[i].name().c_str());
	}

	m_audioOutput1.Create(m_hWND, 800, 20, 150, 23);
	m_onAudioOutputChange1.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioOutputSelectChange1));
	m_audioOutput1.EVENT_SelectChange += m_onAudioInputChange1;
	m_audioOutput2.Create(m_hWND, 1000, 20, 150, 23);
	m_onAudioOutputChange2.Reset(new Delegate<void(INT)>(this, &CMainFrame::OnAudioOutputSelectChange2));
	m_audioOutput2.EVENT_SelectChange += m_onAudioOutputChange2;
	m_audioOutputNames.clear();
	DShow::AudioCapture::GetDevices(m_audioOutputNames);
	for (UINT i = 0; i < m_audioOutputNames.size(); i++)
	{
		m_audioOutput1.AddString(m_audioOutputNames[i].name().c_str());
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
	m_audioInput.Uninitialize();

	m_videoStart.EVENT_Click -= m_onVideoStart;
	m_videoStop.EVENT_Click -= m_onVideoStop;
	m_videoDevice1.EVENT_SelectChange -= m_onVideoChange1;
	m_videoDevice2.EVENT_SelectChange -= m_onVideoChange2;

	m_audioInputStart.EVENT_Click -= m_onAudioInputStart;
	m_audioInputStop.EVENT_Click -= m_onAudioInputStop;
	m_audioInput1.EVENT_SelectChange -= m_onAudioInputChange1;
	m_audioInput2.EVENT_SelectChange -= m_onAudioInputChange2;

	m_audioOutputStart.EVENT_Click -= m_onAudioOutputStart;
	m_audioOutputStop.EVENT_Click -= m_onAudioOutputStop;
	m_audioOutput1.EVENT_SelectChange -= m_onAudioOutputChange1;
	m_audioOutput2.EVENT_SelectChange -= m_onAudioOutputChange2;

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

void CMainFrame::OnAudioInputSelectChange1(INT index)
{
	m_audioInputParams.clear();
	m_audioInput2.ResetContent();
	DShow::AudioCapture::GetDeviceParams(m_audioInputNames[index], m_audioInputParams);
	for (UINT i = 0; i < m_audioInputParams.size(); i++)
	{
		m_audioInput2.AddString(m_audioInputParams[i].ToString().c_str());
	}
}
void CMainFrame::OnAudioInputSelectChange2(INT index)
{
	const DShow::AudioCaptureParam& param = m_audioInputParams[index];
	m_audioInput.Uninitialize();
	m_audioInputCB = BindCallback(&CMainFrame::OnAudioInput, this);
	m_audioInput.Initialize(m_audioInputNames[m_audioInput1.GetCurSel()], m_audioInputCB);
	m_audioInput.Allocate(param);
}

void CMainFrame::OnAudioInput(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{

}

void CMainFrame::OnAudioOutputSelectChange1(INT index)
{
	m_audioOutputParams.clear();
	m_audioOutput2.ResetContent();
	DShow::AudioCapture::GetDeviceParams(m_audioOutputNames[index], m_audioOutputParams);
	for (UINT i = 0; i < m_audioOutputParams.size(); i++)
	{
		m_audioOutput2.AddString(m_audioOutputParams[i].ToString().c_str());
	}
}
void CMainFrame::OnAudioOutputSelectChange2(INT index)
{
	const DShow::AudioCaptureParam& param = m_audioOutputParams[index];
	m_audioOutput.Uninitialize();
	m_audioOutputCB = BindCallback(&CMainFrame::OnAudioOutput, this);
	m_audioOutput.Initialize(m_audioOutputNames[m_audioOutput1.GetCurSel()], m_audioOutputCB);
	m_audioOutput.Allocate(param);
}
void CMainFrame::OnAudioOutputStart(void*, INT)
{

}
void CMainFrame::OnAudioOutputStop(void*, INT)
{

}
void CMainFrame::OnAudioOutput(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{

}


void CMainFrame::OnVideoStart(void*, INT)
{
	m_videoDevice.Start();
}
void CMainFrame::OnVideoStop(void*, INT)
{
	m_videoDevice.Stop();
}

void CMainFrame::OnAudioInputStart(void*, INT)
{
	m_audioInput.Start();
}
void CMainFrame::OnAudioInputStop(void*, INT)
{
	m_audioInput.Stop();
}
