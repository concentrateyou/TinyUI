#include "stdafx.h"
#include "DXFrameUI.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#define FRAME_RATE 30

DXFrameUI::DXFrameUI()
{
}
DXFrameUI::~DXFrameUI()
{
}
BOOL DXFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD DXFrameUI::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}
DWORD DXFrameUI::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}
LPCSTR DXFrameUI::RetrieveClassName()
{
	return TEXT("IQiyiWindowClass");
}
LPCSTR DXFrameUI::RetrieveTitle()
{
	return TEXT("DXFrameUI");
}
HICON DXFrameUI::RetrieveIcon()
{
	return NULL;
}

void DXFrameUI::Resize(INT cx, INT cy)
{
	m_window.SetSize(cx, cy * 3 / 4);
}

LRESULT DXFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	CreateUI();
	CreateTasks();

	CenterWindow(NULL, { 1024, 720 });

	return FALSE;
}
LRESULT DXFrameUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	DestoryTasks();

	PostQuitMessage(0);
	return FALSE;
}
LRESULT DXFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return FALSE;
}

LRESULT DXFrameUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	INT cx = LOWORD(lParam);
	INT cy = HIWORD(lParam);
	Resize(cx, cy);
	return FALSE;
}

BOOL DXFrameUI::CreateUI()
{
	RECT s = { 0 };
	GetClientRect(&s);
	BOOL bRes = m_window.Create(m_hWND, 0, 0, TO_CX(s), TO_CY(s) * 3 / 4);
	if (!bRes)
		return FALSE;
	return TRUE;
}
BOOL DXFrameUI::CreateTasks()
{
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

	vector<DShow::AudioCapture::Name> audioNames;
	DShow::AudioCapture::GetDevices(audioNames);
	vector<DShow::AudioCaptureParam> audioParams;
	DShow::AudioCapture::GetDeviceParams(audioNames[0], audioParams);
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
	//m_publishTask.Reset(new PublishTask(m_audioTask, m_videoTask));

	BOOL bRes = m_renderTask->Initialize(m_window.Handle(), 1280, 720, 30, videoNames[0], videoParam);
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
	/*bRes = m_publishTask->Connect();
	if (!bRes)
	return FALSE;*/

	m_renderTask->Submit();
	m_videoTask->Encode();
	m_audioTask->Encode();
	//m_publishTask->Submit();
}
void DXFrameUI::DestoryTasks()
{
	//m_publishTask->Close(INFINITE);
	m_renderTask->Close(INFINITE);
	m_videoTask->Close();
	m_audioTask->Close();

}