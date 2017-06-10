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
		return TRUE;
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
			if (tag.samplePTS == m_clock.GetBasetPTS())
			{
				m_clock.SetBaseTime(timeGetTime());
			}
			while (m_clock.GetBasetPTS() == -1);
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
			HANDLE handles[2] = { m_events[0],m_events[1] };
			WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		}
	}
}
