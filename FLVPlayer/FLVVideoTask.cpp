#include "stdafx.h"
#include "FLVVideoTask.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVVideoTask::FLVVideoTask(FLVDecodeTask* pTask, HWND hWND)
		:m_pTask(pTask),
		m_hWND(hWND),
		m_hBitmap(NULL),
		m_currentPTS(0),
		m_currentDTS(0)
	{
		m_h264.Reset(new H264Decode());
	}


	FLVVideoTask::~FLVVideoTask()
	{
	}
	BOOL FLVVideoTask::Submit(const TinySize& src, const TinySize& dst, BYTE* bits, LONG size)
	{
		ASSERT(m_h264);
		m_dst = dst;
		m_h264->Initialize(src, dst, BindCallback(&FLVVideoTask::OnH264, this));
		if (m_h264->Open(bits, size))
		{
			m_wait.CreateEvent();
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
	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(1))
				break;
			TinyAutoLock lock(m_pTask->m_videoLock);
			if (!m_pTask->m_videoQueue.empty())
			{
				AVPacket packet = m_pTask->m_videoQueue.front();
				//TRACE("OnMessagePump:%d\n", packet.pts);
				INT offsetPTS = 0;
				if (m_currentPTS != packet.pts)
				{
					offsetPTS = packet.pts - m_currentPTS;
					m_currentPTS = packet.pts;
				}
				INT offsetDTS = 0;
				if (m_currentDTS != packet.dts)
				{
					offsetDTS = packet.dts - m_currentDTS;
					m_currentDTS = packet.dts;
				}
				m_timer.BeginTime();
				m_h264->Decode(packet.bits, packet.size);
				m_pTask->m_videoQueue.pop();
				SAFE_DELETE_ARRAY(packet.bits);
				if (m_pTask->m_videoQueue.size() <= 10)
				{
					m_pTask->m_wait.SetEvent();
				}
				m_timer.EndTime();
				DWORD dwVal = m_timer.GetMillisconds();
				if (dwVal < offsetPTS)
				{
					
					//m_wait.Lock(offsetPTS);
				}
				HDC hDC = GetDC(m_hWND);
				if (hDC != NULL)
				{
					if (m_hBitmap)
					{
						TinyMemDC dc(hDC, m_hBitmap);
						TinyRectangle src = { 0,0,m_dst.cx,m_dst.cy };
						dc.Render(src, src, FALSE);
					}
					ReleaseDC(m_hWND, hDC);
				}
			}
		}
	}
	void FLVVideoTask::OnH264(BYTE* bits, LONG size, LPVOID ps)
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		if (size == m_dst.cx * m_dst.cy * 3)
		{
			HDC hDC = GetDC(m_hWND);
			if (hDC)
			{
				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = m_dst.cx;
				bmi.bmiHeader.biHeight = -m_dst.cy;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = m_dst.cx * m_dst.cy * 3;
				BYTE* pvBits = NULL;
				m_hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
				if (m_hBitmap)
				{
					memcpy(pvBits, bits, size);
				}
				ReleaseDC(m_hWND, hDC);
			}

		}
	}
}