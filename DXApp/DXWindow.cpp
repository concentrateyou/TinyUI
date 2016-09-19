#include "stdafx.h"
#include "DXWindow.h"

#define WINDOW_CLOSE_EVENT      TEXT("WINDOW_CLOSE")
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
	CenterWindow(NULL, { 800, 600 });
	m_graphics.Initialize(m_hWND, 800, 600);

	vector<Media::VideoCapture::Name> videoNames;
	Media::VideoCapture::GetDevices(videoNames);
	vector<Media::VideoCaptureParam> videoParams;
	Media::VideoCapture::GetDeviceParams(videoNames[0], videoParams);
	Media::VideoCaptureParam videoParam;
	for (UINT i = 0; i < videoParams.size(); i++)
	{
		if (videoParams[i].GetSize() == TinySize(800, 600))
		{
			videoParam = videoParams[i];
			break;
		}
	}

	vector<Media::AudioCapture::Name> audioNames;
	Media::AudioCapture::GetDevices(audioNames);
	vector<Media::AudioCaptureParam> audioParams;
	Media::AudioCapture::GetDeviceParams(audioNames[0], audioParams);
	Media::AudioCaptureParam audioParam;
	WAVEFORMATEX defaultWFX = Media::AudioCaptureParam::GetDefaultFormat();
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
	m_mediaCapture.InitializeVideo(videoNames[0], videoParam);
	m_mediaCapture.InitializeAudio(audioNames[0], audioParam);

	m_videoImage.Create(m_graphics.GetD3D(), videoParam.GetSize().cx, videoParam.GetSize().cy, videoParam.GetSize().cx / 2, videoParam.GetSize().cy / 2);
	m_captureTask.Reset(new DX11CaptureTask(&m_graphics.GetD3D(), 800, 600));
	m_renderTask.Reset(new RenderTask(this, FRAME_RATE));
	m_mediaCapture.Start();
	m_captureTask->Submit();
	m_renderTask->Submit();
	m_encodeTask.Reset(new EncodePublishTask(&m_graphics, &m_mediaCapture));
	m_encodeTask->Open(800, 600, 800, 600, FRAME_RATE, 1000, audioParam.GetFormat());
	m_encodeTask->Submit();
	return FALSE;
}
LRESULT DXWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}
LRESULT DXWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	m_mediaCapture.Pause();

	m_captureTask->Exit();
	m_captureTask->Wait(INFINITE);
	m_renderTask->Exit();
	m_renderTask->Wait(INFINITE);
	m_encodeTask->Exit();
	m_encodeTask->Wait(INFINITE);

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

void DXWindow::Render()
{
	m_graphics.BeginScene();
	if (m_mediaCapture.GetVideoPointer())
	{
		TinySize size = m_mediaCapture.GetVideoParam().GetSize();
		m_videoImage.FillImage(m_graphics.GetD3D(), m_mediaCapture.GetVideoPointer());
		m_graphics.DrawImage(m_videoImage, 1, 1);
	}
	m_graphics.DrawImage(m_captureTask->GetTexture(), 1, 1);
	m_graphics.EndScene();
}