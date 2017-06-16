#include "stdafx.h"
#include "MVideoTask.h"

namespace MShow
{
	MVideoTask::MVideoTask(MFLVTask& task, MClock& clock)
		:m_task(task),
		m_clock(clock),
		m_bClose(FALSE)
	{
		m_h264.Reset(new H264Decode());
		ASSERT(m_h264);
	}


	MVideoTask::~MVideoTask()
	{
	}

	BOOL MVideoTask::Submit()
	{
		m_bClose = FALSE;
		m_onAVCDC.Reset(new Delegate<void(BYTE*, LONG)>(this, &MVideoTask::OnAVCDC));
		m_task.EVENT_AVCDCR += m_onAVCDC;
		return TinyTaskBase::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	MPacketQueue& MVideoTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MVideoTask::OnAVCDC(BYTE* bits, LONG size)
	{
		FLV_SCRIPTDATA script = m_task.GetScript();
		TinySize s(static_cast<LONG>(script.width), static_cast<LONG>(script.height));
		if (m_h264->Initialize(s, s))
		{
			m_h264->Open(bits, size);
		}
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		m_task.EVENT_AVCDCR -= m_onAVCDC;
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoTask::OnMessagePump()
	{
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bClose)
				break;
			INT size = m_videoQueue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(3);
				continue;
			}
			BOOL bRes = m_task.GetVideoQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(3);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_h264->Decode(sampleTag, bo, so))
			{
				if (m_clock.GetBasePTS() == -1)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				SAFE_DELETE_ARRAY(sampleTag.bits);
				sampleTag.size = so;
				sampleTag.bits = new BYTE[so];
				memcpy(sampleTag.bits, bo, so);
				sampleTag.samplePTS = m_h264->GetYUV420()->pkt_pts;
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
