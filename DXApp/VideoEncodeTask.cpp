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
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	Closure s = BindCallback(&VideoEncodeTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}

BOOL VideoEncodeTask::Close(DWORD dwMS)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMS);
}

BOOL VideoEncodeTask::Open(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate)
{
	ASSERT(m_renderTask);
	m_dwFPS = dwFPS;
	m_dwVideoRate = dwVideoRate;
	BOOL bRes = m_x264.Open(scale.cx, scale.cy, (INT)dwFPS, (INT)dwVideoRate);
	if (!bRes)
		return FALSE;
	TinySize size = m_renderTask->GetParam()->GetSize();
	m_converter.Reset(new I420Converter(size, scale));
	return TRUE;
}

void VideoEncodeTask::OnMessagePump()
{
	ASSERT(m_renderTask || m_converter);
	for (;;)
	{
		if (m_close.Lock(120))
		{
			OnClose();
			break;
		}
		if (m_converter->BRGAToI420(m_renderTask->GetPointer()))
		{
			m_x264.Encode(m_converter->GetI420());
		}
	}
}

void VideoEncodeTask::OnClose()
{
	m_x264.Close();
}
