#include "stdafx.h"
#include "DXWindow.h"
#include "VideoCapture.h"
#include "AudioInputCapture.h"
#define FRAME_RATE				30

DXWindow::DXWindow()
{
}
DXWindow::~DXWindow()
{
}
BOOL DXWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD DXWindow::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}
DWORD DXWindow::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}
LPCSTR DXWindow::RetrieveClassName()
{
	return TEXT("IQiyiWindowClass");
}
LPCSTR DXWindow::RetrieveTitle()
{
	return TEXT("DXWindow");
}
HICON DXWindow::RetrieveIcon()
{
	return NULL;
}

LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	CenterWindow(NULL, { 1280, 720 });

	vector<DShow::VideoCapture::Name> videoNames;
	DShow::VideoCapture::GetDevices(videoNames);
	vector<DShow::VideoCaptureParam> videoParams;
	DShow::VideoCapture::GetDeviceParams(videoNames[0], videoParams);
	DShow::VideoCaptureParam videoParam;
	for (UINT i = 0; i < videoParams.size(); i++)
	{
		if (videoParams[i].GetSize() == TinySize(1280, 720))
		{
			videoParam = videoParams[i];
			break;
		}
	}

	vector<DShow::AudioInputCapture::Name> audioNames;
	DShow::AudioInputCapture::GetDevices(audioNames);
	vector<DShow::AudioCaptureParam> audioParams;
	DShow::AudioInputCapture::GetDeviceParams(audioNames[0], audioParams);
	DShow::AudioCaptureParam audioParam;
	WAVEFORMATEX defaultWFX = DShow::AudioCaptureParam::GetDefaultFormat();
	for (UINT i = 0; i < audioParams.size(); i++)
	{
		WAVEFORMATEX wfx = audioParams[i].GetFormat();
		if (wfx.nAvgBytesPerSec == defaultWFX.nAvgBytesPerSec &&
			wfx.nBlockAlign == defaultWFX.nBlockAlign &&
			wfx.nChannels == defaultWFX.nChannels &&
			wfx.nSamplesPerSec == defaultWFX.nSamplesPerSec &&
			wfx.wBitsPerSample == defaultWFX.wBitsPerSample &&
			wfx.wFormatTag == defaultWFX.wFormatTag)
		{
			audioParam = audioParams[i];
			break;
		}
	}
	videoParam.SetScale(videoParam.GetSize().cx / 4, videoParam.GetSize().cy / 4);
	m_renderTask.Reset(new RenderTask());
	m_videoTask.Reset(new VideoEncode(m_renderTask));
	m_audioTask.Reset(new AudioEncode());
	m_publishTask.Reset(new PublishTask(m_audioTask, m_videoTask));

	BOOL bRes = m_renderTask->Initialize(m_hWND, 1280, 720, 30, videoNames[0], videoParam);
	if (!bRes)
		return FALSE;
	bRes = m_videoTask->Open({ 800, 600 }, 30, 1000);
	if (!bRes)
		return FALSE;
	bRes = m_audioTask->Initialize(audioNames[0], audioParam);
	if (!bRes)
		return FALSE;
	bRes = m_audioTask->Open(128);
	if (!bRes)
		return FALSE;
	bRes = m_publishTask->Connect();
	if (!bRes)
		return FALSE;

	m_renderTask->Submit();
	m_videoTask->Encode();
	m_audioTask->Encode();
	m_publishTask->Submit();

	return TRUE;
}
LRESULT DXWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	m_publishTask->Close(INFINITE);
	m_renderTask->Close(INFINITE);
	m_videoTask->Close();
	m_audioTask->Close();

	PostQuitMessage(0);
	return FALSE;
}
LRESULT DXWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return FALSE;
}

LRESULT DXWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}