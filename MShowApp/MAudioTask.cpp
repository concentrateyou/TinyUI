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
	}

	BOOL MAudioTask::Submit()
	{
		m_bBreak = FALSE;
		m_task.EVENT_ASC += m_onASC;
		return TinyTaskBase::Submit(BindCallback(&MAudioTask::OnMessagePump, this));
	}

	BOOL MAudioTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_aac.Close();
			m_audioQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;
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
		m_bBreak = !bRes;
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
				Sleep(10);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_task.GetAudioQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(10);
				continue;
			}
			BYTE* bo = NULL;
			LONG  so = 0;
			if (m_aac.Decode(sampleTag.bits, sampleTag.size, bo, so))
			{
				if (m_clock.GetBasePTS() == -1)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				SAFE_DELETE_ARRAY(sampleTag.bits);
				sampleTag.size = so;
				sampleTag.bits = new BYTE[so];
				memcpy(sampleTag.bits, bo, so);
				m_audioQueue.Push(sampleTag);
			}
			else
			{
				SAFE_DELETE_ARRAY(sampleTag.bits);
			}
		}
		m_audioQueue.RemoveAll();
	}
}
