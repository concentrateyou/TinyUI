#include "stdafx.h"
#include "MAudioTask.h"

namespace FLVPlayer
{
	MAudioTask::MAudioTask(MFLVTask& task, MClock& clock, TinyMsgQueue& queue)
		:m_task(task),
		m_clock(clock),
		m_msgqueue(queue),
		m_bBreak(FALSE)
	{
		m_onASC.Reset(new Delegate<void(BYTE*, LONG, WORD, BOOL&)>(this, &MAudioTask::OnASC));
	}


	MAudioTask::~MAudioTask()
	{
		m_task.EVENT_ASC -= m_onASC;
	}

	BOOL MAudioTask::Submit()
	{
		m_bBreak = FALSE;
		m_aac.Close();
		m_audioQueue.RemoveAll();
		m_task.EVENT_ASC += m_onASC;
		return TinyThread::Submit(BindCallback(&MAudioTask::OnMessagePump, this));
	}

	BOOL MAudioTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyThread::Close(dwMS);
		m_aac.Close();
		m_audioQueue.RemoveAll();
		return bRes;
	}

	WAVEFORMATEX* MAudioTask::GetFormat()
	{
		return m_aac.GetFormat();
	}
	MPacketQueue& MAudioTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	void MAudioTask::OnASC(BYTE* bits, LONG size, WORD wBitsPerSample, BOOL& bRes)
	{
		bRes = FALSE;
		m_aac.Close();
		if (m_aac.Open(bits, size, wBitsPerSample))
		{
			bRes = TRUE;
		}
		else
		{
			LOG(ERROR) << "AAC Open FAIL";
		}
		m_bBreak = !bRes;
		m_task.EVENT_ASC -= m_onASC;
	}

	void MAudioTask::OnMessagePump()
	{
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bBreak)
				break;
			INT size = m_audioQueue.GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE)
			{
				Sleep(15);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			LOG(INFO) << "[MAudioTask] Queue Size:" << m_task.GetAudioQueue().GetSize() << " Count:" << m_task.GetAudioQueue().GetCount();
			if (!m_task.GetAudioQueue().Pop(sampleTag))
			{
				Sleep(15);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			LONGLONG prevPTS = sampleTag.samplePTS;
			if (m_aac.Decode(sampleTag, bo, so))
			{
				sampleTag.size = so;
				sampleTag.bits = new BYTE[so];
				if (!sampleTag.bits)
				{
					sampleTag.size = 0;
					LOG(ERROR) << "[MAudioTask] new size:" << so;
				}
				else
				{
					memcpy_s(sampleTag.bits, sampleTag.size, bo, so);
					m_audioQueue.Push(sampleTag);
				}
			}
		}
		m_audioQueue.RemoveAll();
	}
}
