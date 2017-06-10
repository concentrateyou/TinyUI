#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock)
		:m_close(FALSE),
		m_task(task),
		m_clock(clock)
	{
	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Initialize(HWND hWND)
	{
		RECT s = { 0 };
		GetWindowRect(hWND, &s);
		if (!m_d2d.Initialize(hWND, TO_CX(s), TO_CY(s)))
			return FALSE;
		return TRUE;
	}

	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoRenderTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;
			m_task.GetLock().Lock();
			SampleTag sample = { 0 };
			BOOL val = m_task.GetQueue().Pop(sample);
			m_task.GetLock().Unlock();
			if (val && sample.size > 0)
			{
				if (sample.sampleIndex == 1)
				{
					m_clock.SetBaseTime(timeGetTime());
				}

			}
		}
	}
}
