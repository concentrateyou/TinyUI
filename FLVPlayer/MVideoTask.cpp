#include "stdafx.h"
#include "MVideoTask.h"
#include "libyuv.h"
#include "DShowCommon.h"

namespace FLVPlayer
{
	MVideoTask::MVideoTask(MFLVTask& task, MClock& clock, TinyMsgQueue& queue)
		:m_task(task),
		m_clock(clock),
		m_msgqueue(queue),
		m_bBreak(FALSE)
	{
		m_onAVCDC.Reset(new Delegate<void(BYTE*, LONG, BOOL&)>(this, &MVideoTask::OnAVCDC));
		fopen_s(&m_hFile, "D:\\123.yuv", "wb");
	}


	MVideoTask::~MVideoTask()
	{
		m_task.EVENT_AVCDCR -= m_onAVCDC;
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
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
		m_size.SetSize(static_cast<LONG>(script.width), static_cast<LONG>(script.height));
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
			if (m_qsv.Decode(sampleTag, surface1))
			{
				QSV::QSVAllocator* pAllocator = m_qsv.GetAllocator();
				mfxStatus status = pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				if (status == MFX_ERR_NONE)
				{
					/*for (INT i = 0; i < surface1->Info.CropH; i++)
					{
						fwrite(surface1->Data.Y + (surface1->Info.CropY * surface1->Data.Pitch + surface1->Info.CropX) + i * surface1->Data.Pitch, 1, surface1->Info.CropW, m_hFile);
					}
					INT h = surface1->Info.CropH / 2;
					INT w = surface1->Info.CropW;
					for (INT i = 0; i < h; i++)
					{
						for (INT j = 0; j < w; j += 2)
						{
							fwrite(surface1->Data.UV + (surface1->Info.CropY * surface1->Data.Pitch / 2 + surface1->Info.CropX) + i * surface1->Data.Pitch + j, 1, 1, m_hFile);
						}
					}
					for (INT i = 0; i < h; i++)
					{
						for (INT j = 1; j < w; j += 2)
						{
							fwrite(surface1->Data.UV + (surface1->Info.CropY * surface1->Data.Pitch / 2 + surface1->Info.CropX) + i * surface1->Data.Pitch + j, 1, 1, m_hFile);
						}
					}*/
					/*sampleTag.linesize = surface1->Data.Pitch;
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
					}*/
					sampleTag.x = surface1->Info.CropX;
					sampleTag.y = surface1->Info.CropY;
					sampleTag.cx = surface1->Info.Width;
					sampleTag.cy = surface1->Info.Height;
					sampleTag.linesize = surface1->Data.Pitch;
					sampleTag.size = sampleTag.linesize * sampleTag.cy * 3 / 2;
					sampleTag.bits = new BYTE[sampleTag.size];
					if (!sampleTag.bits)
					{
						sampleTag.size = 0;
						LOG(ERROR) << "[MVideoTask] new size:" << sampleTag.size;
					}
					else
					{
						memcpy(sampleTag.bits, surface1->Data.Y, sampleTag.size);
						//libyuv::NV12ToARGB(surface1->Data.Y, surface1->Data.Pitch, surface1->Data.UV, surface1->Data.Pitch, (BYTE*)sampleTag.bits, sampleTag.linesize, m_size.cx, m_size.cy);
						m_videoQueue.Push(sampleTag);
					}
				}
				pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				m_qsv.UnlockSurface(surface1);
			}
			//mfxFrameSurface1* surface1 = NULL;
			//if (m_qsv.Decode(sampleTag, surface1))//FLV解码MORE DATA认为失败
			//{
			//	QSV::QSVAllocator* pAllocator = m_qsv.GetAllocator();
			//	mfxStatus status = pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
			//	if (status == MFX_ERR_NONE)
			//	{
			//		sampleTag.linesize = surface1->Data.Pitch;
			//		sampleTag.cy = surface1->Info.CropH;
			//		sampleTag.size = surface1->Info.CropH * surface1->Data.Pitch;
			//		sampleTag.bits = new BYTE[sampleTag.size];
			//		if (!sampleTag.bits)
			//		{
			//			sampleTag.size = 0;
			//			LOG(ERROR) << "[MVideoTask] new size:" << sampleTag.size;
			//		}
			//		else
			//		{
			//			io_gpu_memcpy(sampleTag.bits, surface1->Data.B, sampleTag.size);
			//			m_videoQueue.Push(sampleTag);
			//		}
			//	}
			//	pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
			//	m_qsv.UnlockSurface(surface1);
			//}
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
