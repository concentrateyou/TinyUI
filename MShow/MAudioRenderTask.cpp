#include "stdafx.h"
#include "MAudioRenderTask.h"
#include "MShow.h"

namespace MShow
{
	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock)
		:m_bInitialize(FALSE),
		m_bBreak(FALSE),
		m_task(task),
		m_clock(clock)
	{

	}

	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& callback)
		:m_bInitialize(FALSE),
		m_bBreak(FALSE),
		m_task(task),
		m_clock(clock),
		m_callback(std::move(callback))
	{
	}

	MAudioRenderTask::~MAudioRenderTask()
	{
	}

	BOOL MAudioRenderTask::Initialize(HWND hWND)
	{
		m_bBreak = FALSE;
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
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MAudioRenderTask::OnMessagePump, this));
	}

	BOOL MAudioRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_events[0].SetEvent();
			m_events[1].SetEvent();
			m_events[2].SetEvent();
			m_player.Close();
			m_bInitialize = FALSE;
			return TRUE;
		}
		return FALSE;
	}


	void MAudioRenderTask::OnMessagePump()
	{
		DWORD	dwOffset = 0;
		SampleTag tag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&tag, sizeof(tag));
			BOOL bRes = m_task.GetAudioQueue().Pop(tag);
			if (!bRes || tag.size <= 0)
			{
				Sleep(15);
				continue;
			}
			if (tag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(MShow::MShowApp::GetInstance().GetQPCTimeMS());
			}
			while (m_clock.GetBasePTS() == -1);
			if (!m_bInitialize)
			{
				m_bInitialize = TRUE;
				TinyTimer timer;
				m_timeQPC.BeginTime();
				if (!m_player.SetFormat(m_task.GetFormat(), tag.size * 3))
					break;
				DSBPOSITIONNOTIFY vals[3];
				vals[0].dwOffset = tag.size - 1;
				vals[0].hEventNotify = m_events[0];
				vals[1].dwOffset = tag.size * 2 - 1;
				vals[1].hEventNotify = m_events[1];
				vals[2].dwOffset = tag.size * 3 - 1;
				vals[2].hEventNotify = m_events[2];
				m_player.SetNotifys(3, vals);
				m_timeQPC.EndTime();
				m_clock.AddBaseTime(static_cast<DWORD>(m_timeQPC.GetMillisconds()));
				LONGLONG ms = MShow::MShowApp::GetInstance().GetQPCTimeMS() - m_clock.GetBaseTime();
				LONG delay = static_cast<LONG>(tag.samplePTS - ms);
				if (timer.Wait(delay, 1000))
				{
					if (tag.size != 4096)
					{
						if (!m_callback.IsNull())
						{
							m_callback(tag.bits + 4, tag.size);
						}
						MShow::MShowApp::GetInstance().SetCurrentAudioTS(tag.samplePTS);
						m_player.Fill(tag.bits + 4, tag.size, dwOffset);
					}
					m_player.Play();
				}
			}
			else
			{
				if (!m_callback.IsNull())
				{
					m_callback(tag.bits + 4, tag.size);
				}
				MShow::MShowApp::GetInstance().SetCurrentAudioTS(tag.samplePTS);
				m_player.Fill(tag.bits + 4, tag.size, dwOffset);
			}
			m_task.GetAudioQueue().Free(tag.bits);
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
