#include "stdafx.h"
#include "MVideoTask.h"

namespace TSPlayer
{
	MVideoTask::MVideoTask(MTSTask& task, MClock& clock, TinyMsgQueue& queue)
		:m_task(task),
		m_clock(clock),
		m_msgqueue(queue),
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
		m_x264.Close();
		m_videoQueue.RemoveAll();
		m_task.EVENT_AVCDCR += m_onAVCDC;
		return TinyThread::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyThread::Close(dwMS);
		m_x264.Close();
		m_videoQueue.RemoveAll();
		return bRes;
	}

	TinySize MVideoTask::GetVideoSize()
	{
		return m_task.GetVideoSize();
	}

	MPacketQueue& MVideoTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MVideoTask::OnAVCDC(BYTE* bits, LONG size, BOOL& bRes)
	{
		bRes = FALSE;
		//TinySize s(1024, 576);
		//TinySize s(1280, 720);
		TinySize s(480, 288);
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
				sampleTag.sampleDTS = sampleTag.samplePTS = m_x264.GetYUV420()->pts;
				sampleTag.size = so;
				sampleTag.bits = new BYTE[so];
				if (!sampleTag.bits)
				{
					sampleTag.size = 0;
				}
				else
				{
					memcpy(sampleTag.bits, bo, so);
					m_videoQueue.Push(sampleTag);
				}
			}
		}
		m_videoQueue.RemoveAll();
	}
}
