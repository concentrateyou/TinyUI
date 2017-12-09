#include "stdafx.h"
#include "VideoEncode.h"
#include "RenderTask.h"

namespace DXApp
{
	VideoEncode::VideoEncode(RenderTask* renderTask)
		:m_renderTask(renderTask)
	{
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}

	VideoEncode::~VideoEncode()
	{

	}

	x264Encode&	VideoEncode::GetEncode()
	{
		return m_x264;
	}
	TinySize VideoEncode::GetSize() const
	{
		return	m_renderTask->GetSize();
	}
	DWORD VideoEncode::GetFPS() const
	{
		return m_dwFPS;
	}
	LONGLONG VideoEncode::Encode()
	{
		ASSERT(m_renderTask);
		m_timer.BeginTime();
		DWORD dwSize = 0;
		BYTE* bits = m_renderTask->GetPointer(dwSize);
		if (bits && m_converter->BRGAToI420(bits))
		{
			m_x264.Encode(m_converter->GetI420());
		}
		m_timer.EndTime();
		return m_timer.GetMillisconds();
	}
	BOOL VideoEncode::Submit()
	{
		return TinyTask::Submit(BindCallback(&VideoEncode::OnMessagePump, this));
	}

	BOOL VideoEncode::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		BOOL bRes = TinyTask::Close(dwMs);
		m_x264.Close();
		return bRes;
	}

	BOOL VideoEncode::Initialize(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate)
	{
		ASSERT(m_renderTask);
		m_dwFPS = dwFPS;
		m_dwVideoRate = dwVideoRate;
		if (!m_x264.Open(scale.cx, scale.cy, (INT)dwFPS, (INT)dwVideoRate))
			return FALSE;
		m_converter.Reset(new I420Converter(scale, scale));
		return TRUE;
	}

	void VideoEncode::OnMessagePump()
	{
		LONGLONG time = 0;
		TinyPerformanceTime timer;
		for (;;)
		{
			DWORD s = 1000 / m_dwFPS;
			s = time > s ? 0 : s - time;
			if (m_close.Lock(s))
			{
				break;
			}
			timer.BeginTime();
			time = this->Encode();
			timer.EndTime();
			//TRACE("Encode Cost:%d\n", timer.GetMillisconds());
		}
	}
}

