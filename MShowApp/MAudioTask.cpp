#include "stdafx.h"
#include "MAudioTask.h"

namespace MShow
{
	MAudioTask::MAudioTask(MReadTask& task, MClock& clock)
		:m_task(task),
		m_clock(clock)
	{
	}


	MAudioTask::~MAudioTask()
	{
	}

	BOOL MAudioTask::Submit()
	{
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&MAudioTask::OnMessagePump, this));
	}

	BOOL MAudioTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	MPacketQueue& MAudioTask::GetQueue()
	{
		return m_audioQueue;
	}

	TinyLock& MAudioTask::GetLock()
	{
		return m_lock;
	}

	AACDecode* MAudioTask::GetAAC()
	{
		return m_task.GetAAC();
	}

	void MAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			INT size = m_task.GetAudioQueue().GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			m_task.GetAudioLock().Lock();
			SampleTag tag = { 0 };
			BOOL val = m_task.GetAudioQueue().Pop(tag);
			m_task.GetAudioLock().Unlock();
			if (val && tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				AACDecode* aac = m_task.GetAAC();
				if (aac != NULL && aac->Decode(tag.bits, tag.size, bo, so))
				{
					if (m_clock.GetBasetPTS() == -1)
					{
						m_clock.SetBasetPTS(tag.samplePTS);
					}
					SAFE_DELETE_ARRAY(tag.bits);
					tag.size = so;
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					m_lock.Lock();
					m_audioQueue.Push(tag);
					m_lock.Unlock();
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
			else
			{
				Sleep(1);
			}
		}
		m_audioQueue.RemoveAll();
	}
}
