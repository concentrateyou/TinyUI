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
	if (!m_close.OpenEvent(EVENT_ALL_ACCESS, FALSE, WINDOW_CLOSE_EVENT) &&
		!m_close.CreateEvent(FALSE, FALSE, WINDOW_CLOSE_EVENT))
	{
		return FALSE;
	}
	if (!m_render.OpenEvent(EVENT_ALL_ACCESS, FALSE, RENDER_FINISH_EVENT) &&
		!m_render.CreateEvent(FALSE, FALSE, RENDER_FINISH_EVENT))
	{
		return FALSE;
	}
	if (!m_publish.OpenEvent(EVENT_ALL_ACCESS, FALSE, PUBLISH_FINISH_EVENT) &&
		!m_publish.CreateEvent(FALSE, FALSE, PUBLISH_FINISH_EVENT))
	{
		return FALSE;
	}
	return TRUE;
}

LRESULT DXWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	BuildEvents();
	CenterWindow(NULL, { 800, 600 });
	m_graphics.Initialize(m_hWND, 800, 600);

	vector<Media::VideoCapture::Name> names;
	Media::VideoCapture::GetDevices(names);
	vector<Media::VideoCaptureParam> params;
	Media::VideoCapture::GetDeviceParams(names[0], params);
	Media::VideoCaptureParam param;
	for (UINT i = 0; i < params.size(); i++)
	{
		if (params[i].GetSize() == TinySize(800, 600))
		{
			param = params[i];
			break;
		}
	}
	vector<Media::AudioCapture::Name> names1;
	Media::AudioCapture::GetDevices(names1);
	vector<Media::AudioCaptureParam> params1;
	Media::AudioCapture::GetDeviceParams(names1[0], params1);
	Media::AudioCaptureParam param1;
	WAVEFORMATEX defaultWFX = Media::AudioCaptureParam::GetDefaultFormat();
	for (UINT i = 0; i < params1.size(); i++)
	{
		WAVEFORMATEX wfx = params1[i].GetFormat();
		if (wfx.nAvgBytesPerSec == defaultWFX.nAvgBytesPerSec &&
			wfx.nBlockAlign == defaultWFX.nBlockAlign &&
			wfx.nChannels == defaultWFX.nChannels &&
			wfx.nSamplesPerSec == defaultWFX.nSamplesPerSec &&
			wfx.wBitsPerSample == defaultWFX.wBitsPerSample &&
			wfx.wFormatTag == defaultWFX.wFormatTag)
		{
			param1 = params1[i];
			break;
		}
	}
	m_graphics.InitializeVideo(names[0], param);
	m_graphics.InitializeAudio(names1[0], param1);

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
	m_publish.SetEvent();
	m_render.SetEvent();
	m_close.SetEvent();
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