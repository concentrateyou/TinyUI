#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask()
	:m_size(0),
	m_bits(NULL)
{

}

RenderTask::~RenderTask()
{

}

BOOL RenderTask::Initialize(HWND hWND, INT cx, INT cy, DWORD dwFPS, const VideoCapture::Name& name, const VideoCaptureParam& param)
{
	m_dwFPS = dwFPS;
	m_videoParam = param;
	m_deviceName = name;
	//初始化DX
	BOOL bRes = m_graphics.Initialize(hWND, cx, cy);
	if (!bRes)
		return FALSE;
	TinySize size = m_videoParam.GetSize();
	TinySize scale = m_videoParam.GetScale();
	bRes = m_image.Create(m_graphics.GetD3D(), size.cx, size.cy, scale.cx, scale.cy);
	if (!bRes)
		return FALSE;
	//游戏捕获
	m_captureTask.Reset(new DX11CaptureTask(&m_graphics.GetD3D(), cx * 2 / 3, cy * 2 / 3));
	//初始化视频捕获
	bRes = m_capture.Initialize(m_deviceName);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(m_videoParam);
	if (!bRes)
		return FALSE;
	return TRUE;
}

VideoCapture* RenderTask::GetCapture()
{
	return &m_capture;
}

BYTE* RenderTask::GetPointer()
{
	return m_graphics.GetPointer();
}

VideoCaptureParam* RenderTask::GetParam()
{
	return &m_videoParam;
}

BOOL RenderTask::Close(DWORD dwMs)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMs);
}

BOOL RenderTask::Submit()
{
	ASSERT(m_captureTask);
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	m_captureTask->Submit();
	m_capture.Start();
	Closure s = BindCallback(&RenderTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}

DWORD RenderTask::Render()
{
	m_timer.BeginTime();
	m_graphics.BeginScene();
	BYTE* s = m_capture.GetPointer();
	if (s != NULL)
	{
		m_image.FillImage(m_graphics.GetD3D(), s);
		TinySize size = m_graphics.GetD3D().GetSize();
		m_graphics.DrawImage(m_image, size.cx * 2 / 3 + 1, 1);
	}
	if (m_captureTask != NULL)
	{
		m_graphics.DrawImage(m_captureTask->GetTexture(), 1, 1);
	}
	m_graphics.EndScene();
	this->OnRender(m_graphics.GetPointer(), m_graphics.GetSize(), (FLOAT)m_dwFPS);
	m_timer.EndTime();
	DWORD dwTime = m_timer.GetMicroseconds() / 1000;
	return dwTime;
}

void RenderTask::OnRender(BYTE* bits, LONG size, FLOAT ts)
{
	EVENT_RENDER(bits, size, ts);
}

void RenderTask::OnExit()
{
	ASSERT(m_captureTask);
	m_capture.Uninitialize();
	m_captureTask->Close(INFINITE);
}
void RenderTask::OnMessagePump()
{
	DWORD dwTime = 0;
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		s = dwTime > s ? 0 : s - dwTime;
		if (m_close.Lock(s))
		{
			OnExit();
			break;
		}
		dwTime = this->Render();
	}
}
