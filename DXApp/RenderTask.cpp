#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask()
	:m_size(0)
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
	m_dx11CaptureTask.Reset(new DX11CaptureTask(&m_graphics.GetD3D(), cx, cy));
	//初始化视频捕获
	m_videoCB = BindCallback(&RenderTask::OnVideo, this);
	bRes = m_capture.Initialize(m_deviceName, m_videoCB);
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

void RenderTask::OnVideo(BYTE* bits, LONG size,FLOAT ts, LPVOID ps)
{
	if (m_size != size)
	{
		m_size = size;
		m_bits.Reset(new BYTE[m_size]);
		m_queue.Initialize(ROUNDUP_POW_2(m_size * 3));
	}
	m_queue.Write(bits, size);
}

BYTE* RenderTask::GetPointer() 
{
	return m_graphics.GetPointer();
}

VideoCaptureParam* RenderTask::GetParam()
{
	return &m_videoParam;
}

BOOL RenderTask::Submit()
{
	ASSERT(m_dx11CaptureTask);
	m_dx11CaptureTask->Submit();
	m_capture.Start();
	Closure s = BindCallback(&RenderTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}
DWORD RenderTask::Render()
{
	m_timer.BeginTime();
	if (m_graphics.BeginScene())
	{
		if (!m_bits.IsEmpty())
		{
			m_queue.Read(m_bits, m_size);
			m_image.FillImage(m_graphics.GetD3D(), m_bits);
			m_graphics.DrawImage(m_image, 1, 1);
		}
		if (m_dx11CaptureTask)
		{
			m_graphics.DrawImage(m_dx11CaptureTask->GetTexture(), 1, 1);
		}
		m_graphics.EndScene();
	}
	m_timer.EndTime();
	LONGLONG s = m_timer.GetMicroseconds();
	return s / 1000;
}
void RenderTask::Exit()
{
	m_signal.SetEvent();
}
void RenderTask::OnExit()
{
	ASSERT(m_dx11CaptureTask);
	m_capture.Uninitialize();
	m_dx11CaptureTask->Exit();
	m_dx11CaptureTask->Wait(INFINITE);
}
void RenderTask::OnMessagePump()
{
	DWORD offset = 0;
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		s = offset > s ? 0 : s - offset;
		if (m_signal.Lock(s))
		{
			OnExit();
			break;
		}
		offset = this->Render();
	}
}
