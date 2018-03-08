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
		SampleTag tag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&tag, sizeof(tag));
			LOG(INFO) << "[MAudioRenderTask] Queue Size:" << m_task.GetAudioQueue().GetSize() << " Count:" << m_task.GetAudioQueue().GetCount();
			if (!m_task.GetAudioQueue().Pop(tag))
			{
				Sleep(15);
				continue;
			}
			m_clock.SetAudioPTS(tag.samplePTS);
			if (!m_bInitialize)
			{
				if (!m_audio.Open(m_task.GetFormat()))
				{
					break;
				}
				if (!m_audio.Start())
				{
					break;
				}
				m_bInitialize = TRUE;
				if (!m_callback.IsNull())
				{
					m_callback(tag.bits, tag.size);
				}
				m_audio.Play(tag.bits, tag.size, 5000);
			}
			else
			{
				if (!m_callback.IsNull())
				{
					m_callback(tag.bits, tag.size);
				}
				m_audio.Play(tag.bits, tag.size, 5000);
			}
			m_clock.UnlockVideo();
			SAFE_DELETE_ARRAY(tag.bits);
		}
		m_task.GetAudioQueue().RemoveAll();
		m_bInitialize = FALSE;
		CoUninitialize();
	}
}
