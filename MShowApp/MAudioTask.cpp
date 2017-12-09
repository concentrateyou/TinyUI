#include "stdafx.h"
#include "MAudioTask.h"

namespace MShow
{
	MAudioTask::MAudioTask(MFLVTask& task, MClock& clock)
		:m_task(task),
		m_clock(clock),
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
		m_task.EVENT_ASC += m_onASC;
		return TinyTask::Submit(BindCallback(&MAudioTask::OnMessagePump, this));
	}

	BOOL MAudioTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTask::Close(dwMS))
		{
			m_aac.Close();
			m_task.GetAudioQueue().RemoveAll();
			m_audioQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}

	WAVEFORMATEX* MAudioTask::GetFormat()
	{
		return m_aac.GetFormat();
	}

	MPacketAllocQueue& MAudioTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	void MAudioTask::OnASC(BYTE* bits, LONG size, WORD wBitsPerSample, BOOL& bRes)
	{
		TRACE("Audio OnASC\n");
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
				Sleep(5);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (!m_task.GetAudioQueue().Pop(sampleTag))
			{
				Sleep(5);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_aac.Decode(sampleTag, bo, so))
			{
				if (m_clock.GetBasePTS() == INVALID_TIME)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				if (m_audioQueue.GetAllocSize() == 0)
				{
					INT count = MAX_AUDIO_QUEUE_SIZE / so + 1;
					m_audioQueue.Initialize(count, so + 4);
				}
				sampleTag.size = so;
				sampleTag.bits = static_cast<BYTE*>(m_audioQueue.Alloc());
				memcpy_s(sampleTag.bits + 4, sampleTag.size, bo, so);
				m_audioQueue.Push(sampleTag);
			}
		}
		m_audioQueue.RemoveAll();
	}
}
