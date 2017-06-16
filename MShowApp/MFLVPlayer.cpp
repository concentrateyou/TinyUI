#include "stdafx.h"
#include "MFLVPlayer.h"

namespace MShow
{
	MFLVPlayer::MFLVPlayer(DX2D& d2d)
		:m_d2d(d2d),
		m_task(m_clock),
		m_audioTask(m_task, m_clock),
		m_audioRenderTask(m_audioTask, m_clock),
		m_videoTask(m_task, m_clock),
		m_videoRenderTask(m_videoTask, m_clock, BindCallback(&MFLVPlayer::OnVideo, this))
	{
	}

	MFLVPlayer::~MFLVPlayer()
	{
	}

	BOOL MFLVPlayer::Open(LPCSTR pzURL)
	{
		if (!m_d2d.IsValid())
			return FALSE;
		if (!m_task.Initialize(pzURL))
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Initialize(m_d2d.GetHWND()))
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		FLV_SCRIPTDATA script = m_task.GetScript();
		m_size.cx = static_cast<LONG>(script.width);
		m_size.cy = static_cast<LONG>(script.height);
		HRESULT hRes = m_d2d.GetContext()->CreateBitmap(D2D1::SizeU(m_size.cx, m_size.cy),
			(const void *)NULL,
			0,
			&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&m_bitmap);
		return SUCCEEDED(hRes);
	}

	ID2D1Bitmap1* MFLVPlayer::GetBitmap()
	{
		return m_bitmap;
	}

	void MFLVPlayer::OnVideo(BYTE* bits, LONG size)
	{
		if (m_bitmap != NULL)
		{
			m_bitmap->CopyFromMemory(NULL, bits, m_size.cx * 4);
			m_d2d.BeginDraw();
			TinyRectangle s;
			::GetClientRect(m_d2d.GetHWND(), &s);
			TinyPoint pos = s.Position();
			TinySize size = s.Size();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<FLOAT>(m_size.cx), static_cast<FLOAT>(m_size.cy) };
			m_d2d.GetContext()->DrawBitmap(m_bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			m_d2d.EndDraw();
		}
	}

	BOOL MFLVPlayer::Close()
	{
		m_bitmap.Release();
		BOOL bRes = TRUE;
		if (m_task.IsValid())
			bRes &= m_task.Close(INFINITE);
		if (m_videoTask.IsValid())
			bRes &= m_videoTask.Close(INFINITE);
		if (m_videoRenderTask.IsValid())
			bRes &= m_videoRenderTask.Close(INFINITE);
		if (m_audioTask.IsValid())
			bRes &= m_audioTask.Close(INFINITE);
		if (m_audioRenderTask.IsValid())
			bRes &= m_audioRenderTask.Close(INFINITE);
		return bRes;
	}
}

