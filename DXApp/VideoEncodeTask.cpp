#include "stdafx.h"
#include "VideoEncodeTask.h"
#include "RenderTask.h"


VideoEncodeTask::VideoEncodeTask(RenderTask* renderTask)
	:m_renderTask(renderTask)
{
	ASSERT(m_renderTask);
	m_render.Reset(new Delegate<void(BYTE*, LONG)>(this, &VideoEncodeTask::OnRender));
	m_renderTask->EVENT_RENDER += m_render;
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
	m_renderTask->EVENT_RENDER -= m_render;
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

void VideoEncodeTask::OnRender(BYTE* bits, LONG size)
{
	TinyScopedReferencePtr<RawSample> sample(new RawSample(size));
	sample->Fill(bits, size);
	m_queue.Add(sample);
}

void VideoEncodeTask::OnMessagePump()
{
	ASSERT(m_renderTask || m_converter);
	DWORD dwTime = 0;
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		s = dwTime > s ? 0 : s - dwTime;
		if (m_close.Lock(s))
		{
			OnClose();
			break;
		}
		m_timer.BeginTime();
		if (!m_queue.IsEmpty())
		{
			RawSample* sample = m_queue.GetSample();
			if (sample == NULL)
				TRACE("Video:sample == NULL");
			if (m_converter->BRGAToI420(sample->Bits))
			{
				m_x264.Encode(m_converter->GetI420());
			}
			m_queue.Remove();
		}
		m_timer.EndTime();
		dwTime = m_timer.GetMicroseconds() / 1000;
	}
}

void VideoEncodeTask::OnClose()
{
	m_x264.Close();
}


