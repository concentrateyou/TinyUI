#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask()
{

}
RenderTask::~RenderTask()
{

}
BOOL RenderTask::Initialize(HWND hWND, INT cx, INT cy, const VideoCapture::Name& name, const VideoCaptureParam& param)
{
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
	bRes = m_videoCapture.Initialize(m_deviceName);
	if (!bRes)
		return FALSE;
	bRes = m_videoCapture.Allocate(m_videoParam);
	if (!bRes)
		return FALSE;
	return TRUE;
}
BOOL RenderTask::Submit()
{
	ASSERT(m_dx11CaptureTask);
	m_dx11CaptureTask->Submit();
	m_videoCapture.Start();
	Closure s = BindCallback(&RenderTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}
void RenderTask::Render()
{
	m_graphics.BeginScene();
	if (m_videoCapture.GetPointer())
	{
		TinySize size = m_videoParam.GetSize();
		m_image.FillImage(m_graphics.GetD3D(), m_videoCapture.GetPointer());
		m_graphics.DrawImage(m_image, 1, 1);
	}
	if (m_dx11CaptureTask)
	{
		m_graphics.DrawImage(m_dx11CaptureTask->GetTexture(), 1, 1);
	}
	m_graphics.EndScene();
}
void RenderTask::Exit()
{
	m_signal.SetEvent();
}
void RenderTask::OnExit()
{
	ASSERT(m_dx11CaptureTask);
	m_videoCapture.Pause();
	m_videoCapture.Uninitialize();
	m_dx11CaptureTask->Exit();
	m_dx11CaptureTask->Wait(INFINITE);
}
void RenderTask::MessagePump()
{
	for (;;)
	{
		DWORD s = 1000 / m_videoParam.GetRate();
		if (m_signal.Lock(s))
		{
			OnExit();
			break;
		}
		this->Render();
	}
}
