#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock, DX2D& d2d, TinyUI::Callback<void(ID2D1Bitmap1*, INT)>&& callback)
		:m_task(task),
		m_clock(clock),
		m_d2d(d2d),
		m_callback(std::move(callback))
	{
	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Initialize()
	{
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
			INT ms = timeGetTime() - m_clock.GetBaseTime();
			INT delay = static_cast<INT>(tag.samplePTS - ms);
			Sleep(delay < 0 ? 0 : delay);
			OnRender(tag.bits, tag.size, delay);
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}

	void MVideoRenderTask::OnRender(BYTE* bits, LONG size, INT delay)
	{
		if (!m_callback.IsNull() && m_bitmap != NULL)
		{
			TinySize s = m_task.GetSize();
			m_bitmap->CopyFromMemory(NULL, bits, s.cx * 4);
			m_callback(m_bitmap, delay);
		}
	}
}
