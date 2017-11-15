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

	BOOL MAudioRenderTask::Initialize()
	{
		m_bBreak = FALSE;
		return TRUE;
	}

	BOOL MAudioRenderTask::SetVolume(DWORD dwVolume)
	{
		return m_audio.SetVolume(dwVolume);
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
		m_bInitialize = FALSE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_audio.Close();
			return TRUE;
		}
		return FALSE;
	}


	void MAudioRenderTask::OnMessagePump()
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		TinyPerformanceTime	 timeQPC;
		TinyPerformanceTimer timer;
		SampleTag tag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&tag, sizeof(tag));
			if (!m_task.GetAudioQueue().Pop(tag))
			{
				Sleep(15);
				continue;
			}
			if (tag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(MShow::MShowApp::GetInstance().GetQPCTimeMS());
			}
			while (m_clock.GetBasePTS() == INVALID_TIME);
			if (!m_bInitialize)
			{
				timeQPC.BeginTime();
				if (!m_audio.Open(m_task.GetFormat()))
				{
					TRACE("Audio Open FAIL");
					LOG(ERROR) << "Audio Open FAIL";
					break;
				}
				m_bInitialize = TRUE;
				m_audio.Start();
				timeQPC.EndTime();
				m_clock.AddBaseTime(static_cast<DWORD>(timeQPC.GetMillisconds()));
				LONGLONG ms = MShow::MShowApp::GetInstance().GetQPCTimeMS() - m_clock.GetBaseTime();
				LONG delay = static_cast<LONG>(tag.samplePTS - ms);
				if (timer.Waiting(delay, 1000))
				{
					MShow::MShowApp::GetInstance().SetCurrentAudioTS(static_cast<LONGLONG>(tag.samplePTS) + m_task.GetBasePTS());
					if (!m_callback.IsNull())
					{
						m_callback(tag.bits + 4, tag.size);
					}
					m_audio.Play(tag.bits + 4, tag.size, 5000);
				}
			}
			else
			{
				MShow::MShowApp::GetInstance().SetCurrentAudioTS(static_cast<LONGLONG>(tag.samplePTS) + m_task.GetBasePTS());
				if (!m_callback.IsNull())
				{
					m_callback(tag.bits + 4, tag.size);
				}
				m_audio.Play(tag.bits + 4, tag.size, 5000);
			}
			m_task.GetAudioQueue().Free(tag.bits);
		}
		m_bInitialize = FALSE;
		CoUninitialize();
	}
}
