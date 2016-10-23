#include "stdafx.h"
#include "VideoEncode.h"
#include "RenderTask.h"


VideoEncode::VideoEncode(RenderTask* renderTask)
	:m_renderTask(renderTask)
{
	ASSERT(m_renderTask);
	m_render.Reset(new Delegate<void(BYTE*, LONG, FLOAT)>(this, &VideoEncode::OnRender));
	m_renderTask->EVENT_RENDER += m_render;
}

VideoEncode::~VideoEncode()
{

}

x264Encode*	VideoEncode::GetEncode()
{
	return &m_x264;
}

VideoCapture* VideoEncode::GetCapture()
{
	ASSERT(m_renderTask);
	return m_renderTask->GetCapture();
}
VideoCaptureParam* VideoEncode::GetParam()
{
	ASSERT(m_renderTask);
	return m_renderTask->GetParam();
}

BOOL VideoEncode::Encode()
{
	return TRUE;
}

BOOL VideoEncode::Close()
{
	m_x264.Close();
	return TRUE;
}

BOOL VideoEncode::Open(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate)
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

void VideoEncode::OnRender(BYTE* bits, LONG size, FLOAT ts)
{
	if (m_converter->BRGAToI420(bits))
	{
		m_x264.Encode(m_converter->GetI420());
	}
}

