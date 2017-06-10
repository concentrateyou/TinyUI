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
		return m_queue;
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
			INT size = m_queue.GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			SampleTag tag = { 0 };
			BOOL val = m_task.GetAudioQueue().Pop(tag);
			if (!val || tag.size <= 0)
				continue;
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
				m_queue.Push(tag);
			}
			else
			{
				SAFE_DELETE_ARRAY(tag.bits);
			}
		}
		m_queue.RemoveAll();
	}
}
