#include "stdafx.h"
#include "DXWindow.h"

#define WINDOW_CLOSE_EVENT      TEXT("WINDOW_CLOSE")
#define RENDER_FINISH_EVENT     TEXT("RENDER_FINISH")
#define PUBLISH_FINISH_EVENT    TEXT("PUBLISH_FINISH")

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

BOOL DXWindow::BuildEvents()
{
	return TRUE;
}

LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	BuildEvents();
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

	m_converter.Reset(new I420Converter(TinySize(800, 600), TinySize(800, 600)));

	m_videoImage.Create(m_graphics.GetD3D(), videoParam.GetSize().cx, videoParam.GetSize().cy, videoParam.GetSize().cx / 4, videoParam.GetSize().cy / 4);

	m_captureTask.Reset(new DX11CaptureTask(&m_graphics.GetD3D(), 800, 600));

	Closure s = BindCallback(&DXWindow::OnRender, this);
	m_renderTask.Reset(new RenderTask(s));

	s = BindCallback(&DXWindow::OnPublish, this);
	m_publishTask.Reset(new PublishTask(s));

	m_mediaCapture.Start();

	m_publisher.Connect("rtmp://10.121.86.127/live/test");
	m_x264.Open(videoParam.GetSize().cx, videoParam.GetSize().cy, videoParam.GetRate());
	m_aac.Open(audioParam.GetFormat());

	m_captureTask->Submit();
	m_renderTask->Submit();
	m_publishTask->Submit();

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

	m_captureTask->Quit();
	m_captureTask->Wait(INFINITE);
	m_publishTask->Quit();
	m_publishTask->Wait(INFINITE);
	m_renderTask->Quit();
	m_renderTask->Wait(INFINITE);

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

void DXWindow::OnPublish()
{
	ASSERT(m_converter);
	Sleep(25);
	if (m_converter->BRGAToI420(m_graphics.GetPointer()))
	{
		EncoderPacket packet;
		m_x264.Encode(m_converter->GetI420(), packet);
	}
}

void DXWindow::OnRender()
{
	Sleep(25);
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