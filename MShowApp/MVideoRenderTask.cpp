#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock)
		:m_close(FALSE),
		m_task(task),
		m_clock(clock)
	{
	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Initialize(HWND hWND)
	{
		RECT s = { 0 };
		GetWindowRect(hWND, &s);
		if (!m_d2d.Initialize(hWND, TO_CX(s), TO_CY(s)))
			return FALSE;
		TinySize size = m_task.GetSize();
		HRESULT hRes = m_d2d.GetContext()->CreateBitmap(D2D1::SizeU(size.cx, size.cy),
			(const void *)NULL,
			0,
			&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&m_bitmap);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	BOOL MVideoRenderTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}


	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoRenderTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;
			m_task.GetLock().Lock();
			SampleTag sample = { 0 };
			BOOL val = m_task.GetQueue().Pop(sample);
			m_task.GetLock().Unlock();
			if (val && sample.size > 0)
			{
				if (sample.sampleIndex == 1)
				{
					m_clock.SetBaseTime(timeGetTime());
				}
				DWORD dwMS = timeGetTime() - m_clock.GetBaseTime();
				INT offset = static_cast<INT>(sample.samplePTS - dwMS);
				Sleep(offset < 0 ? 0 : offset);
				OnRender(sample.bits, sample.size);
				SAFE_DELETE_ARRAY(sample.bits);
			}
		}
	}

	void MVideoRenderTask::OnRender(BYTE* bits, LONG size)
	{
		TinyPerformanceTimer timer;
		timer.BeginTime();
		if (m_d2d.BeginDraw())
		{
			TinySize s = m_task.GetSize();
			m_bitmap->CopyFromMemory(NULL, bits, s.cx * 4);
			m_d2d.GetContext()->DrawBitmap(m_bitmap);
			m_d2d.EndDraw();
		}
		timer.EndTime();
		TRACE("MVideoRenderTask - Render:%d\n", timer.GetMillisconds());
	}
}
