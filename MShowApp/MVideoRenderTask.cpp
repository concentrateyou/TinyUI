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

	BOOL GetTag(MVideoTask& task, SampleTag& tag)
	{
		task.GetLock().Lock();
		BOOL bRes = task.GetQueue().Pop(tag);
		task.GetLock().Unlock();
		return bRes;
	}

	void MVideoRenderTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close)
				break;
			SampleTag tag = { 0 };
			if (!GetTag(m_task, tag) || tag.size <= 0)
				continue;
			if (tag.samplePTS == m_clock.GetBasetPTS())
			{
				m_clock.SetBaseTime(timeGetTime());
			}
			while (m_clock.GetBasetPTS() == -1);
			DWORD dwMS = timeGetTime() - m_clock.GetBaseTime();
			INT offset = static_cast<INT>(tag.samplePTS - dwMS);
			Sleep(offset < 0 ? 0 : offset);
			OnRender(tag.bits, tag.size);
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}

	void MVideoRenderTask::OnRender(BYTE* bits, LONG size)
	{
		if (m_d2d.BeginDraw() && bits)
		{
			TinySize s = m_task.GetSize();
			m_bitmap->CopyFromMemory(NULL, bits, s.cx * 4);
			m_d2d.GetContext()->DrawBitmap(m_bitmap);
			m_d2d.EndDraw();
		}
	}
}
