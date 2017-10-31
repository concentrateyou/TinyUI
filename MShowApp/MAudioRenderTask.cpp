#include "stdafx.h"
#include "MAudioRenderTask.h"
#include "MShowApp.h"

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

	BOOL MAudioRenderTask::Initialize()
	{
		m_bBreak = FALSE;
		m_audio.Close();
		return TRUE;
	}

	BOOL MAudioRenderTask::SetVolume(DWORD volume)
	{
		return m_audio.SetVolume(volume);
	}

	BOOL MAudioRenderTask::Submit()
	{
		m_bBreak = FALSE;
		m_audio.Close();
		return TinyTaskBase::Submit(BindCallback(&MAudioRenderTask::OnMessagePump, this));
	}

	BOOL MAudioRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_audio.Close();
			m_bInitialize = FALSE;
			return TRUE;
		}
		return FALSE;
	}


	void MAudioRenderTask::OnMessagePump()
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		TinyPerformanceTimer timer;
		TinyPerformanceTime	 timeQPC;
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
				timeQPC.BeginTime();
				if (!m_audio.Open(m_task.GetFormat()))
					break;
				m_audio.Start();
				m_audio.SetVolume(0);
				timeQPC.EndTime();
				m_clock.AddBaseTime(static_cast<DWORD>(timeQPC.GetMillisconds()));
				LONGLONG ms = MShow::MShowApp::GetInstance().GetQPCTimeMS() - m_clock.GetBaseTime();
				LONG delay = static_cast<LONG>(tag.samplePTS - ms);
				if (timer.Wait(delay, 1000))
				{
					m_audio.Play(tag.bits + 4, tag.size, 1000);
				}
			}
			else
			{
				m_audio.Play(tag.bits + 4, tag.size, 1000);
			}
			if (!m_callback.IsNull())
			{
				m_callback(tag.bits + 4, tag.size);
			}
			m_task.GetAudioQueue().Free(tag.bits);
		}
		CoUninitialize();
	}
}
