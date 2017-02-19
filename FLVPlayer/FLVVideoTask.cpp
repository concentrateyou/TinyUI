#include "stdafx.h"
#include "FLVVideoTask.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVVideoTask::FLVVideoTask(FLVDecodeTask* pTask, TinyLock& lock, HWND hWND)
		:m_pTask(pTask),
		m_hWND(hWND),
		m_lock(lock),
		m_currentPTS(0)
	{
		m_h264.Reset(new H264Decode());
	}

	FLVVideoTask::~FLVVideoTask()
	{
	}
	BOOL FLVVideoTask::Submit(const TinySize& srcsize, const TinySize& dstsize, BYTE* bits, LONG size)
	{
		ASSERT(m_h264);
		m_dstsize = dstsize;
		m_h264->Initialize(srcsize, dstsize);
		if (m_h264->Open(bits, size))
		{
			m_close.CreateEvent();
			return TinyTaskBase::Submit(BindCallback(&FLVVideoTask::OnMessagePump, this));
		}
		return FALSE;
	}
	BOOL FLVVideoTask::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		return TRUE;
	}
	void FLVVideoTask::Push(const SampleTag& tag)
	{
		m_lock.Lock();
		m_queue.push(tag);
		m_lock.Unlock();
	}
	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			LONG pts = 0;
			if (OnProcessTag(pts))
			{
				LONG offset = 0;
				if (m_currentPTS != pts)
				{
					offset = pts - m_currentPTS - 6;
					m_currentPTS = pts;
					TRACE("video-offset:%d\n", offset);
					Sleep(offset < 0 ? 0 : offset);
				}
			}
		}
	}
	BOOL FLVVideoTask::OnProcessTag(LONG& pts)
	{
		TinyAutoLock lock(m_lock);
		if (m_queue.empty())
			return FALSE;
		m_timer.BeginTime();
		SampleTag tag = m_queue.front();
		m_queue.pop();
		BYTE* bo = NULL;
		LONG  so = 0;
		if (m_h264->Decode(tag.bits, tag.size, tag, bo, so))
		{
			this->OnRender(bo, so);
		}
		SAFE_DELETE_ARRAY(tag.bits);
		AVFrame* yuv420 = m_h264->GetYUV420();
		m_timer.EndTime();
		pts = yuv420->pkt_pts - m_timer.GetMillisconds();
		return TRUE;
	}
	void FLVVideoTask::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_dstsize.cx * m_dstsize.cy * 3);
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_dstsize.cx;
			bmi.bmiHeader.biHeight = -m_dstsize.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_dstsize.cx * m_dstsize.cy * 3;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap)
			{
				memcpy(pvBits, bits, size);
				TinyMemDC dc(hDC, hBitmap);
				TinyRectangle src = { 0,0,m_dstsize.cx,m_dstsize.cy };
				dc.Render(src, src, FALSE);
				SAFE_DELETE_OBJECT(hBitmap);
			}
			ReleaseDC(m_hWND, hDC);
		}
	}
}