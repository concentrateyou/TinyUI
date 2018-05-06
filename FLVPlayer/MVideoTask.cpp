#include "stdafx.h"
#include "MVideoTask.h"

namespace FLVPlayer
{
	MVideoTask::MVideoTask(MFLVTask& task, MClock& clock, TinyMsgQueue& queue)
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
		m_qsv.Close();
		//m_x264.Close();
		m_videoQueue.RemoveAll();
		m_task.EVENT_AVCDCR += m_onAVCDC;
		return TinyThread::Submit(BindCallback(&MVideoTask::OnMessagePump, this));
	}

	BOOL MVideoTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyThread::Close(dwMS);
		m_qsv.Close();
		//m_x264.Close();
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
		FLV_SCRIPTDATA& script = m_task.GetScript();
		TinySize s(static_cast<LONG>(script.width), static_cast<LONG>(script.height));
		m_qsv.Close();
		bRes = m_qsv.Open(bits, size);
		if (!bRes)
		{
			LOG(ERROR) << "qsv Open FAIL";
		}
		m_bBreak = !bRes;
		/*m_x264.Close();
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
		}*/
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
			LOG(INFO) << "[MVideoTask] Queue Size:" << m_task.GetVideoQueue().GetSize() << " Count:" << m_task.GetVideoQueue().GetCount();
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (!m_task.GetVideoQueue().Pop(sampleTag))
			{
				Sleep(15);
				continue;
			}
			mfxFrameSurface1* surface1 = NULL;
			if (m_qsv.Decode(sampleTag, surface1))//FLV解码MORE DATA认为失败
			{
				QSV::QSVAllocator* pAllocator = m_qsv.GetAllocator();
				mfxStatus status = pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				if (status == MFX_ERR_NONE)
				{
					sampleTag.linesize = surface1->Data.Pitch;
					sampleTag.cy = surface1->Info.CropH;
					sampleTag.size = surface1->Info.CropH * surface1->Data.Pitch;
					sampleTag.bits = new BYTE[sampleTag.size];
					if (!sampleTag.bits)
					{
						sampleTag.size = 0;
						LOG(ERROR) << "[MVideoTask] new size:" << sampleTag.size;
					}
					else
					{
						io_gpu_memcpy(sampleTag.bits, surface1->Data.B, sampleTag.size);
						m_videoQueue.Push(sampleTag);
					}
				}
				pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				m_qsv.UnlockSurface(surface1);
			}
			//BYTE* bo = NULL;
			//LONG  so = 0;
			//if (m_x264.Decode(sampleTag, bo, so) == 0)//FLV解码MORE DATA认为失败
			//{
			//	sampleTag.sampleDTS = sampleTag.samplePTS = m_x264.GetYUV420()->pts;
			//	sampleTag.size = so;
			//	sampleTag.bits = new BYTE[so];
			//	if (!sampleTag.bits)
			//	{
			//		sampleTag.size = 0;
			//		LOG(ERROR) << "[MVideoTask] new size:" << so;
			//	}
			//	else
			//	{
			//		memcpy(sampleTag.bits, bo, so);
			//		m_videoQueue.Push(sampleTag);
			//	}
			//}
		}
		m_videoQueue.RemoveAll();
	}
}
