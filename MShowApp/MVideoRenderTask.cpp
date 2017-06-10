#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock)
		:m_task(task),
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
		m_size.cx = TO_CX(s);
		m_size.cy = TO_CY(s);
		if (!m_d2d.Initialize(hWND, m_size.cx, m_size.cy))
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
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}


	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoRenderTask::OnMessagePump()
	{
		SampleTag tag = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			ZeroMemory(&tag, sizeof(tag));
			BOOL val = m_task.GetQueue().Pop(tag);
			if (!val || !tag.bits || tag.size <= 0)
			{
				Sleep(3);
				continue;
			}
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
		if (m_d2d.BeginDraw())
		{
			TinySize s = m_task.GetSize();
			m_bitmap->CopyFromMemory(NULL, bits, s.cx * 4);
			D2D_RECT_F dst = { 0.0F,0.0F,static_cast<FLOAT>(m_size.cx),static_cast<FLOAT>(m_size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<FLOAT>(s.cx),static_cast<FLOAT>(s.cy) };
			m_d2d.GetContext()->DrawBitmap(m_bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			m_d2d.EndDraw();
		}
	}
}
