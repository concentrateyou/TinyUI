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
	}

	BOOL MVideoTask::Submit()
	{
		m_bBreak = FALSE;
		m_task.EVENT_AVCDCR += m_onAVCDC;
		return TinyTaskBase::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		m_task.EVENT_AVCDCR -= m_onAVCDC;
		if (TinyTaskBase::Close(dwMS))
		{
			m_h264.Close();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}

	MPacketQueue& MVideoTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MVideoTask::OnAVCDC(BYTE* bits, LONG size, BOOL& bRes)
	{
		bRes = FALSE;
		FLV_SCRIPTDATA& script = m_task.GetScript();
		TinySize s(static_cast<LONG>(script.width), static_cast<LONG>(script.height));
		m_h264.Close();
		if (m_h264.Initialize(s, s))
		{
			bRes = m_h264.Open(bits, size);
		}
		m_bBreak = !bRes;
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
				Sleep(10);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_task.GetVideoQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(10);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_h264.Decode(sampleTag, bo, so))
			{
				if (m_clock.GetBasePTS() == -1)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				SAFE_DELETE_ARRAY(sampleTag.bits);
				sampleTag.size = so;
				sampleTag.bits = new BYTE[so];
				memcpy(sampleTag.bits, bo, so);
				sampleTag.samplePTS = m_h264.GetYUV420()->pkt_pts;
				sampleTag.sampleDTS = sampleTag.samplePTS;
				m_videoQueue.Push(sampleTag);
			}
			else
			{
				SAFE_DELETE_ARRAY(sampleTag.bits);
			}
		}
		m_videoQueue.RemoveAll();
	}
}
