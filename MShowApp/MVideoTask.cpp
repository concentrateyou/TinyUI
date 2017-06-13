#include "stdafx.h"
#include "MVideoTask.h"

namespace MShow
{
	MVideoTask::MVideoTask(MFLVTask& task, MClock& clock)
		:m_task(task),
		m_clock(clock)
	{
	}


	MVideoTask::~MVideoTask()
	{
	}

	BOOL MVideoTask::Submit()
	{
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	MPacketQueue& MVideoTask::GetQueue()
	{
		return m_queue;
	}

	H264Decode* MVideoTask::GetH264()
	{
		return m_task.GetH264();
	}

	TinySize MVideoTask::GetSize() const
	{
		return TinySize(static_cast<LONG>(m_task.GetScript().width), static_cast<LONG>(m_task.GetScript().height));
	}

	void MVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			INT size = m_queue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = { 0 };
			BOOL val = m_task.GetVideoQueue().Pop(tag);
			if (!val || !tag.bits || tag.size <= 0)
			{
				Sleep(3);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_task.GetH264()->Decode(tag, bo, so))
			{
				if (m_clock.GetBasePTS() == -1)
				{
					m_clock.SetBasePTS(tag.samplePTS);
				}
				SAFE_DELETE_ARRAY(tag.bits);
				tag.size = so;
				tag.bits = new BYTE[so];
				memcpy(tag.bits, bo, so);
				tag.samplePTS = m_task.GetH264()->GetYUV420()->pkt_pts;
				tag.sampleDTS = tag.samplePTS;
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
