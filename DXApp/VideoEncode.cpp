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
		return	m_renderTask->GetGraphics().GetSize();
	}
	DWORD VideoEncode::GetFPS() const
	{
		return m_dwFPS;
	}
	DWORD VideoEncode::Encode()
	{
		ASSERT(m_renderTask);
		m_timer.BeginTime();
		DWORD dwSize = 0;
		BYTE* bits = m_renderTask->GetGraphics().GetPointer(dwSize);
		if (m_converter->BRGAToI420(bits))
		{
			m_x264.Encode(m_converter->GetI420());
		}
		m_timer.EndTime();
		return m_timer.GetMillisconds();
	}
	BOOL VideoEncode::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&VideoEncode::OnMessagePump, this));
	}

	BOOL VideoEncode::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		BOOL bRes = TinyTaskBase::Close(dwMs);
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
		m_converter.Reset(new I420Converter(GetSize(), scale));
		return TRUE;
	}

	void VideoEncode::OnMessagePump()
	{
		DWORD dwTime = 0;
		for (;;)
		{
			DWORD s = 1000 / m_dwFPS;
			s = dwTime > s ? 0 : s - dwTime;
			if (m_close.Lock(s))
			{
				break;
			}
			dwTime = this->Encode();
		}
	}
}

