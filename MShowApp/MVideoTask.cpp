#include "stdafx.h"
#include "MVideoTask.h"

namespace MShow
{
	MVideoTask::MVideoTask(MFLVTask& task, MClock& clock)
		:m_task(task),
		m_clock(clock),
		m_bBreak(FALSE)
	{
		m_onAVCDC.Reset(new Delegate<void(BYTE*, LONG, BOOL&)>(this, &MVideoTask::OnAVCDC));
	}


	MVideoTask::~MVideoTask()
	{
		m_task.EVENT_AVCDCR -= m_onAVCDC;
	}

	BOOL MVideoTask::Submit()
	{
		m_bBreak = FALSE;
		m_task.EVENT_AVCDCR += m_onAVCDC;
		return TinyTask::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{

		m_bBreak = TRUE;
		if (TinyTask::Close(dwMS))
		{
			m_x264.Close();
			m_task.GetVideoQueue().RemoveAll();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}

	MPacketAllocQueue& MVideoTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MVideoTask::OnAVCDC(BYTE* bits, LONG size, BOOL& bRes)
	{
		bRes = FALSE;
		FLV_SCRIPTDATA& script = m_task.GetScript();
		TinySize s(static_cast<LONG>(script.width), static_cast<LONG>(script.height));
		m_x264.Close();
		if (m_x264.Initialize(s, s))
		{
			bRes = m_x264.Open(bits, size);
			if (!bRes)
			{
				LOG(ERROR) << "x264 Open FAIL";
			}
			m_bBreak = !bRes;
		}
		else
		{
			LOG(ERROR) << "x264 Initialize FAIL";
		}
		m_task.EVENT_AVCDCR -= m_onAVCDC;
	}


	void MVideoTask::OnMessagePump()
	{
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			INT size = m_videoQueue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(15);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (!m_task.GetVideoQueue().Pop(sampleTag))
			{
				Sleep(15);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_x264.Decode(sampleTag, bo, so) == 0)
			{
				if (m_videoQueue.GetAllocSize() == 0)
				{
					INT count = MAX_VIDEO_QUEUE_SIZE / so + 1;
					m_videoQueue.Initialize(count, so + 4);
				}
				sampleTag.sampleDTS = sampleTag.samplePTS = m_x264.GetYUV420()->pts;
				sampleTag.size = so;
				sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
				memcpy(sampleTag.bits + 4, bo, so);
				if (m_clock.GetBasePTS() == INVALID_TIME)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				m_videoQueue.Push(sampleTag);
			}
		}
		m_videoQueue.RemoveAll();
	}
}
