#include "stdafx.h"
#include "TSPlayer.h"
#include "MVideoRenderTask.h"

namespace TSPlayer
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

	BOOL MVideoRenderTask::OnCopy(BYTE* bits, LONG size)
	{
		if (bits == NULL || size <= 0)
			return FALSE;
		if (!m_graphics.IsActive())
		{
			HRESULT hRes = S_OK;
			if (m_graphics.GetDX9().CheckReason(D3DERR_DEVICENOTRESET))
			{
				if (m_graphics.Reset())
				{
					m_image.Destory();
					if (!m_image.Create(m_graphics.GetDX9(), m_videoSize.cx, m_videoSize.cy, NULL))
					{
						LOG(ERROR) << "[MPreviewController] [OnDraw]" << " Create FAIL";
						return FALSE;
					}
				}
			}
		}
		else
		{
			if (!m_image.Copy(bits, size))
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
			if (!m_graphics.DrawImage(&m_image))
			{
				LOG(ERROR) << "[MPreviewController] [OnDraw]" << "DrawImage FAIL";
				return FALSE;
			}
			if (!m_graphics.GetRenderView()->EndDraw())
			{
				LOG(ERROR) << "[MPreviewController] [OnDraw]" << "EndDraw FAIL";
				return FALSE;
			}
		}
		return TRUE;
	}
	void MVideoRenderTask::OnMessagePump()
	{
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
				if (!m_image.Create(m_graphics.GetDX9(), m_videoSize.cx, m_videoSize.cy, NULL))
				{
					LOG(ERROR) << "Image2D Create FAIL";
					SAFE_DELETE_ARRAY(sampleTag.bits);
					break;
				}
				TinyRectangle s;
				GetClientRect(m_hWND, &s);
				//m_image.SetScale(s.Size());
				m_bInitialize = TRUE;
			}
			OnCopy(sampleTag.bits, sampleTag.size);
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
