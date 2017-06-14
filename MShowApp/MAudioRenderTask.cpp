#include "stdafx.h"
#include "MAudioRenderTask.h"

namespace MShow
{
	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock)
		:m_bInitialize(FALSE),
		m_task(task),
		m_clock(clock)
	{
	}

	MAudioRenderTask::~MAudioRenderTask()
	{
	}

	BOOL MAudioRenderTask::Initialize(HWND hWND)
	{
		if (!m_player.Initialize(hWND))
			return FALSE;
		m_events[0].CreateEvent(TRUE, FALSE);
		m_events[1].CreateEvent(TRUE, FALSE);
		m_events[2].CreateEvent(TRUE, FALSE);
		return TRUE;
	}

	BOOL MAudioRenderTask::SetVolume(LONG volume)
	{
		return m_player.SetVolume(volume);
	}

	BOOL MAudioRenderTask::Submit()
	{
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&MAudioRenderTask::OnMessagePump, this));
	}

	BOOL MAudioRenderTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}


	void MAudioRenderTask::OnMessagePump()
	{
		DWORD	dwOffset = 0;
		SampleTag tag = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			ZeroMemory(&tag, sizeof(tag));
			BOOL val = m_task.GetQueue().Pop(tag);
			if (!val || !tag.bits || tag.size <= 0)
			{
				Sleep(3);
				continue;
			}
			if (tag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(timeGetTime());
			}
			while (m_clock.GetBasePTS() == -1);
			if (!m_bInitialize)
			{
				m_bInitialize = TRUE;
				TinyPerformanceTimer timer;
				timer.BeginTime();
				if (!m_player.SetFormat(&m_task.GetAAC()->GetFormat(), tag.size * 3))
					break;
				DSBPOSITIONNOTIFY vals[3];
				vals[0].dwOffset = tag.size - 1;
				vals[0].hEventNotify = m_events[0];
				vals[1].dwOffset = tag.size * 2 - 1;
				vals[1].hEventNotify = m_events[1];
				vals[2].dwOffset = tag.size * 3 - 1;
				vals[2].hEventNotify = m_events[2];
				m_player.SetNotifys(3, vals);
				timer.EndTime();
				m_clock.AddBaseTime(timer.GetMillisconds());
				INT ms = timeGetTime() - m_clock.GetBaseTime();
				INT delay = tag.samplePTS - ms;
				Sleep(delay < 0 ? 0 : delay);
				if (tag.size != 4096)
				{
					m_player.Fill(tag.bits, tag.size, dwOffset);
				}
				m_player.Play();
			}
			else
			{
				m_player.Fill(tag.bits, tag.size, dwOffset);
				SAFE_DELETE_ARRAY(tag.bits);
			}
			HANDLE handles[3] = { m_events[0],m_events[1],m_events[2] };
			HRESULT hRes = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
			switch (hRes)
			{
			case WAIT_OBJECT_0:
				dwOffset = 0;
				break;
			case WAIT_OBJECT_0 + 1:
				dwOffset = m_player.GetSize() / 3 * 1;
				break;
			case WAIT_OBJECT_0 + 2:
				dwOffset = m_player.GetSize() / 3 * 2;
				break;
			}
		}
	}
}
