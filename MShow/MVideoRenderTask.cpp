#include "stdafx.h"
#include "MVideoRenderTask.h"
#include "MShow.h"


namespace MShow
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
		return TinyTaskBase::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}

	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyTaskBase::Close(dwMS);
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
				TRACE("[MVideoRenderTask] Waiting 40\n");
				LOG(INFO) << "[MVideoRenderTask] Waiting 40";
				timer.Waiting(40, 1000);
				if (bRendering)
				{
					m_clock.AddBaseTime(40);
				}
				continue;
			}
			if (sampleTag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(MShow::MShowApp::GetInstance().GetQPCTimeMS());
				TRACE("[MVideoRenderTask] BaseTime:%lld\n", m_clock.GetBaseTime());
				TRACE("[MVideoRenderTask] samplePTS:%lld\n", sampleTag.samplePTS);
				LOG(INFO) << "[MVideoRenderTask] BaseTime:" << m_clock.GetBaseTime();
				LOG(INFO) << "[MVideoRenderTask] samplePTS:" << sampleTag.samplePTS;
			}
			while (m_clock.GetBasePTS() == INVALID_TIME);
			bRendering = TRUE;
			if (!m_bInitialize)
			{
				m_videoSize = m_task.GetVideoSize();
				if (!m_image.Create(m_graphics.GetDX9(), m_videoSize.cx, m_videoSize.cy, NULL))
				{
					LOG(ERROR) << "Image2D Create FAIL";
				}
				m_bInitialize = TRUE;
			}
			OnCopy(sampleTag.bits, sampleTag.size);
			LONG systemMS = static_cast<LONG>(MShow::MShowApp::GetInstance().GetQPCTimeMS() - m_clock.GetBaseTime());
			INT delay = static_cast<INT>(sampleTag.samplePTS - systemMS);
			if (delay >= 50)
			{
				TRACE("Video Delay:%d  samplePTS:%lld systemMS:%d \n", delay, sampleTag.samplePTS, systemMS);
				LOG(INFO) << "Video Delay:" << delay << " samplePTS:" << sampleTag.samplePTS << " systemMS:" << systemMS;
			}
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
