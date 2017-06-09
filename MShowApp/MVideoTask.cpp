#include "stdafx.h"
#include "MVideoTask.h"

namespace MShow
{
	MVideoTask::MVideoTask(MReadTask& task, MClock& clock)
		:m_close(FALSE),
		m_task(task),
		m_clock(clock)
	{
	}


	MVideoTask::~MVideoTask()
	{
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	MPacketQueue& MVideoTask::GetQueue()
	{
		return m_videoQueue;
	}

	TinyLock& MVideoTask::GetLock()
	{
		return m_lock;
	}

	H264Decode* MVideoTask::GetH264()
	{
		return m_task.GetH264();
	}


	void MVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;
			m_task.GetVideoLock().Lock();
			INT size = m_task.GetVideoQueue().GetSize();
			m_task.GetVideoLock().Unlock();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			m_task.GetVideoLock().Lock();
			SampleTag tag = { 0 };
			BOOL val = m_task.GetVideoQueue().Pop(tag);
			m_task.GetVideoLock().Unlock();
			if (val && tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_task.GetH264()->Decode(tag, bo, so))
				{
					SAFE_DELETE_ARRAY(tag.bits);
					tag.size = so;
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.samplePTS = m_task.GetH264()->GetYUV420()->pkt_pts;
					tag.sampleDTS = tag.samplePTS;
					if (m_clock.GetFirstPTS() == -1)
					{
						m_clock.SetFirstPTS(tag.samplePTS);
					}
					m_lock.Lock();
					m_videoQueue.Push(tag);
					m_lock.Unlock();
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
		m_videoQueue.RemoveAll();
	}
}
