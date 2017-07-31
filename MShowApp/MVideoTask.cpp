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
		return TinyTaskBase::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_qsv.Close();
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
		m_qsv.Close();
		bRes = m_qsv.Open(bits, size);
		m_bBreak = !bRes;
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
				Sleep(50);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_task.GetVideoQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(50);
				continue;
			}
			mfxFrameSurface1* surface1 = NULL;
			if (m_qsv.Decode(sampleTag, surface1) && sampleTag.size > 0)
			{
				QSV::QSVAllocator* pAllocator = m_qsv.GetAllocator();
				pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				sampleTag.size = surface1->Info.CropH * surface1->Data.Pitch;
				if (m_videoQueue.GetAllocSize() == 0)
				{
					INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
					m_videoQueue.Initialize(count, sampleTag.size + 4);
				}
				sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
				memcpy(sampleTag.bits + 4, surface1->Data.B, sampleTag.size);
				pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				m_qsv.UnlockSurface(surface1);
				if (m_clock.GetBasePTS() == -1)
				{
					m_clock.SetBasePTS(sampleTag.samplePTS);
				}
				m_videoQueue.Push(sampleTag);
			}
		}
		m_videoQueue.RemoveAll();
	}
}
