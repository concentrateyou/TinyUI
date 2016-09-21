#include "stdafx.h"
#include "VideoEncodeTask.h"
#include "RenderTask.h"


VideoEncodeTask::VideoEncodeTask(RenderTask* renderTask)
	:m_renderTask(renderTask)
{
}

VideoEncodeTask::~VideoEncodeTask()
{

}

x264Encode*	VideoEncodeTask::GetEncode()
{
	return &m_x264;
}

VideoCapture* VideoEncodeTask::GetCapture()
{
	ASSERT(m_renderTask);
	return m_renderTask->GetCapture();
}
VideoCaptureParam* VideoEncodeTask::GetParam()
{
	ASSERT(m_renderTask);
	return m_renderTask->GetParam();
}

BOOL VideoEncodeTask::Submit()
{
	Closure s = BindCallback(&VideoEncodeTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}

void VideoEncodeTask::Exit()
{
	m_signal.SetEvent();
}

BOOL VideoEncodeTask::Open(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate)
{
	ASSERT(m_renderTask);
	m_dwFPS = dwFPS;
	m_dwVideoRate = dwVideoRate;
	TinySize size = m_renderTask->GetParam()->GetSize();
	BOOL bRes = m_x264.Open(size.cx, size.cy, (INT)dwFPS, (INT)dwVideoRate);
	if (!bRes)
		return FALSE;
	m_converter.Reset(new I420Converter(size, scale));
	return TRUE;
}

void VideoEncodeTask::OnMessagePump()
{
	ASSERT(m_renderTask || m_converter);
	for (;;)
	{
		if (m_signal.Lock(0))
		{
			OnExit();
			break;
		}
		if (m_converter->BRGAToI420(m_renderTask->GetPointer()))
		{
			m_x264.Encode(m_converter->GetI420());
		}
	}
}

void VideoEncodeTask::OnExit()
{
	m_x264.Close();
}
