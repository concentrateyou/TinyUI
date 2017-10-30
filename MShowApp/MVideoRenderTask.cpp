#include "stdafx.h"
#include "MVideoRenderTask.h"
#include "MShowApp.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& copyCB, Closure&& renderCB)
		:m_task(task),
		m_clock(clock),
		m_copyCB(std::move(copyCB)),
		m_renderCB(std::move(renderCB)),
		m_bBreak(FALSE)
	{

	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Submit()
	{
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}

	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoRenderTask::OnMessagePump()
	{
		TinyPerformanceTime		timeQPC;
		TinyPerformanceTimer	timer;
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_task.GetVideoQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(25);
				continue;
			}
			if (sampleTag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(MShow::MShowApp::GetInstance().GetQPCTimeMS());
			}
			while (m_clock.GetBasePTS() == -1);
			if (!m_copyCB.IsNull())
			{
				m_copyCB(sampleTag.bits + 4, sampleTag.size);
			}
			LONG systemMS = static_cast<LONG>(MShow::MShowApp::GetInstance().GetQPCTimeMS() - m_clock.GetBaseTime());
			INT delay = static_cast<INT>(sampleTag.samplePTS - systemMS);
			if (timer.Wait(delay, 1000))
			{
				if (!m_renderCB.IsNull())
				{
					m_renderCB();
				}
			}
			m_task.GetVideoQueue().Free(sampleTag.bits);
		}
	}
}
