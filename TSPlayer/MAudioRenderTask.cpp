#include "stdafx.h"
#include "MAudioRenderTask.h"
#include "TSPlayer.h"

namespace TSPlayer
{
	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock, TinyMsgQueue& queue)
		:m_bInitialize(FALSE),
		m_bBreak(FALSE),
		m_task(task),
		m_clock(clock),
		m_msgqueue(queue)
	{

	}

	MAudioRenderTask::MAudioRenderTask(MAudioTask& task, MClock& clock, TinyMsgQueue& queue, TinyUI::Callback<void(BYTE*, LONG)>&& callback)
		:m_bInitialize(FALSE),
		m_bBreak(FALSE),
		m_task(task),
		m_clock(clock),
		m_msgqueue(queue),
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
		m_bInitialize = FALSE;
		m_audio.Close();
		return TinyTask::Submit(BindCallback(&MAudioRenderTask::OnMessagePump, this));
	}

	BOOL MAudioRenderTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyTask::Close(dwMS);
		m_bInitialize = FALSE;
		m_audio.Close();
		return bRes;
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
			LOG(INFO) << "[MAudioRenderTask] Queue Size:" << m_task.GetAudioQueue().GetSize() << " Count:" << m_task.GetAudioQueue().GetCount();
			if (!m_task.GetAudioQueue().Pop(tag))
			{
				LOG(INFO) << "[MAudioRenderTask] SetCurrentAudioTS 0";
				LOG(ERROR) << "[MAudioRenderTask] No Audio Sleep(15)";
				TRACE("[MAudioRenderTask] No Audio Sleep(15)\n");
				LOG(ERROR) << "[MAudioRenderTask] No Audio Sleep(15)";
				Sleep(15);
				continue;
			}
			if (tag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(GetQPCTimeMS());
				TRACE("[MAudioRenderTask] BaseTime:%lld\n", m_clock.GetBaseTime());
				TRACE("[MAudioRenderTask] samplePTS:%lld\n", tag.samplePTS);
				LOG(INFO) << "[MAudioRenderTask] BaseTime:" << m_clock.GetBaseTime();
				LOG(INFO) << "[MAudioRenderTask] samplePTS:" << tag.samplePTS;
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
				if (!m_audio.Start())
				{
					TRACE("Audio Start FAIL");
					LOG(ERROR) << "Audio Start FAIL";
					break;
				}
				m_bInitialize = TRUE;
				timeQPC.EndTime();
				m_clock.AddBaseTime(static_cast<DWORD>(timeQPC.GetMillisconds()));
				LONGLONG ms = GetQPCTimeMS() - m_clock.GetBaseTime();
				LONG delay = static_cast<LONG>(tag.samplePTS - ms);
				if (timer.Waiting(delay, 1000))
				{
					if (!m_callback.IsNull())
					{
						m_callback(tag.bits, tag.size);
					}
					m_audio.Play(tag.bits, tag.size, 5000);
				}
			}
			else
			{
				if (!m_callback.IsNull())
				{
					m_callback(tag.bits, tag.size);
				}
				m_audio.Play(tag.bits, tag.size, 5000);
			}
			SAFE_DELETE_ARRAY(tag.bits);
		}
		m_task.GetAudioQueue().RemoveAll();
		m_bInitialize = FALSE;
		CoUninitialize();
	}
}
