#include "stdafx.h"
#include "MAudioRenderTask.h"

namespace MShow
{
	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock)
		:m_close(FALSE),
		m_bInitialize(FALSE),
		m_task(task),
		m_clock(clock)
	{
	}

	MAudioRenderTask::~MAudioRenderTask()
	{
	}

	BOOL MAudioRenderTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MAudioRenderTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;
			m_task.GetLock().Lock();
			SampleTag tag = { 0 };
			BOOL val = m_task.GetQueue().Pop(tag);
			m_task.GetLock().Unlock();
			if (val && tag.size > 0)
			{
				if (tag.sampleIndex == 1)
				{
					m_clock.SetBaseTime(timeGetTime());
				}
				if (!m_bInitialize)
				{
					m_timer.BeginTime();
					m_bInitialize = TRUE;
					if (!m_player.SetFormat(&m_task.GetAAC()->GetFormat(), tag.size * 2))
						break;
					m_events[0].CreateEvent(TRUE, FALSE);
					m_events[1].CreateEvent(TRUE, FALSE);
					DSBPOSITIONNOTIFY vals[2];
					vals[0].dwOffset = tag.size - 1;
					vals[0].hEventNotify = m_events[0];
					vals[1].dwOffset = tag.size * 2 - 1;
					vals[1].hEventNotify = m_events[1];
					m_player.SetPositions(2, vals);
					m_timer.EndTime();
					m_clock.AddBaseTime(m_timer.GetMillisconds());
					DWORD dwMS = timeGetTime() - m_clock.GetBaseTime();
					INT offset = tag.samplePTS - dwMS;
					Sleep(offset < 0 ? 0 : offset);
					if (tag.size != 4096)
					{
						m_player.Fill(tag.bits, tag.size);
					}
					m_player.Fill(tag.bits, tag.size);
					SAFE_DELETE_ARRAY(tag.bits);
					m_player.Play();
				}
				else
				{
					m_player.Fill(tag.bits, tag.size);
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}
}
