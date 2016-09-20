#include "stdafx.h"
#include "VideoEncodeTask.h"


VideoEncodeTask::VideoEncodeTask(DXGraphics* graphics, MediaCapture* capture)
	:m_graphics(graphics),
	m_capture(capture),
	m_dwTimestamp(0)
{
}

VideoEncodeTask::~VideoEncodeTask()
{

}

x264Encode*	VideoEncodeTask::GetEncode()
{
	return &m_x264;
}

BOOL VideoEncodeTask::Submit()
{
	Closure s = BindCallback(&VideoEncodeTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void VideoEncodeTask::Exit()
{
	m_signal.SetEvent();
}

BOOL VideoEncodeTask::Open(INT cx, INT cy, INT scaleX, INT scaleY, DWORD dwFPS, DWORD dwVideoRate)
{
	m_dwFPS = dwFPS;
	BOOL bRes = m_x264.Open(cx, cy, (INT)dwFPS, (INT)dwVideoRate);
	if (!bRes)
		return FALSE;
	m_converter.Reset(new I420Converter(TinySize(cx, cy), TinySize(scaleX, scaleY)));
	return TRUE;
}

void VideoEncodeTask::MessagePump()
{
	for (;;)
	{
		if (m_signal.Lock(0))
		{
			m_x264.Close();
			break;
		}
		if (m_converter->BRGAToI420(m_graphics->GetPointer()))
		{
			m_x264.Encode(m_converter->GetI420());
		}
	}
}
