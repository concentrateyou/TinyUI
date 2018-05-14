#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace FLVPlayer
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock, TinyMsgQueue& queue)
		:m_task(task),
		m_clock(clock),
		m_msgqueue(queue),
		m_bBreak(FALSE),
		m_bInitialize(FALSE),
		m_hWND(NULL)
	{

	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Initialize(HWND hWND)
	{
		m_hWND = hWND;
		TinyRectangle s;
		GetClientRect(m_hWND, &s);
		return m_graphics.Initialize(hWND, s.Size());
	}

	BOOL MVideoRenderTask::Submit()
	{
		m_bBreak = FALSE;
		m_bInitialize = FALSE;
		m_image.Destory();
		return TinyThread::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}

	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyThread::Close(dwMS);
		m_bInitialize = FALSE;
		m_image.Destory();
		return bRes;
	}

	BOOL MVideoRenderTask::OnCopy(SampleTag& tag)
	{
		//const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV
		BYTE* pY = tag.bits;
		BYTE* pNV = tag.bits + tag.cy * tag.linesize;
		if (!m_image.Copy(m_graphics.GetDX11(), pY, tag.linesize, pNV, tag.linesize))
		{
			LOG(ERROR) << "[MPreviewController] [OnDraw]" << " Copy FAIL";
			return FALSE;
		}
		m_graphics.SetRenderView(NULL);
		if (!m_graphics.GetRenderView()->BeginDraw())
		{
			LOG(ERROR) << "[MPreviewController] [OnDraw]" << "BeginDraw FAIL";
			return FALSE;
		}
		if (!m_graphics.DrawImageNV12BT601(m_image))
		{
			LOG(ERROR) << "[MPreviewController] [OnDraw]" << "DrawImage FAIL";
			return FALSE;
		}
		if (!m_graphics.GetRenderView()->EndDraw())
		{
			LOG(ERROR) << "[MPreviewController] [OnDraw]" << "EndDraw FAIL";
			return FALSE;
		}
		return TRUE;
	}
	void MVideoRenderTask::OnMessagePump()
	{
		BOOL bRendering = FALSE;
		TinyPerformanceTimer timer;
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			LOG(INFO) << "[MVideoRenderTask] Queue Size:" << m_task.GetVideoQueue().GetSize() << " Count:" << m_task.GetVideoQueue().GetCount();
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (!m_task.GetVideoQueue().Pop(sampleTag))
			{
				Sleep(15);
				continue;
			}
			if (!m_bInitialize)
			{
				m_videoSize = m_task.GetVideoSize();
				if (!m_image.Create(m_graphics.GetDX11(), m_videoSize.cx, m_videoSize.cy))
				{
					LOG(ERROR) << "Image2D Create FAIL";
					SAFE_DELETE_ARRAY(sampleTag.bits);
					break;
				}
				RECT s;
				GetClientRect(m_hWND, &s);
				m_image.SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(s)) / m_image.GetSize().x, static_cast<FLOAT>(TO_CY(s)) / m_image.GetSize().y));
				m_bInitialize = TRUE;
			}
			OnCopy(sampleTag);
			while (isnan(m_clock.GetClock()));
			INT delay = static_cast<INT>(sampleTag.samplePTS - m_clock.GetClock());
			if (timer.Waiting(delay, 100))
			{
				m_graphics.Present();
			}
			SAFE_DELETE_ARRAY(sampleTag.bits);
		}
		m_task.GetVideoQueue().RemoveAll();
		m_bInitialize = FALSE;
	}
}
