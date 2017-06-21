#include "stdafx.h"
#include "MFLVPlayer.h"

namespace MShow
{
	MFLVPlayer::MFLVPlayer(DX2D& dx2d, Callback<void(BYTE*, LONG)>&& callback)
		:m_dx2d(dx2d),
		m_callback(std::move(callback)),
		m_task(m_clock),
		m_audioTask(m_task, m_clock),
		m_audioRenderTask(m_audioTask, m_clock),
		m_videoTask(m_task, m_clock),
		m_videoRenderTask(m_videoTask, m_clock, BindCallback(&MFLVPlayer::OnVideo, this)),
		m_bPlaying(FALSE)
	{
	}

	MFLVPlayer::~MFLVPlayer()
	{
	}

	BOOL MFLVPlayer::IsPlaying() const
	{
		return m_bPlaying;
	}

	BOOL MFLVPlayer::Open(LPCSTR pzURL)
	{
		this->Close();
		if (!m_task.Initialize(pzURL))
			return FALSE;
		if (!m_audioRenderTask.Initialize(m_dx2d.GetHWND()))
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		FLV_SCRIPTDATA script = m_task.GetScript();
		m_size.cx = static_cast<LONG>(script.width);
		m_size.cy = static_cast<LONG>(script.height);
		HRESULT hRes = m_dx2d.GetContext()->CreateBitmap(D2D1::SizeU(m_size.cx, m_size.cy),
			(const void *)NULL,
			0,
			&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&m_bitmap);
		m_bPlaying = SUCCEEDED(hRes);
		return m_bPlaying;
	}

	ID2D1Bitmap1* MFLVPlayer::GetBitmap()
	{
		return m_bitmap;
	}

	void MFLVPlayer::OnVideo(BYTE* bits, LONG lsize)
	{
		if (m_bitmap != NULL)
		{
			m_bitmap->CopyFromMemory(NULL, bits, m_size.cx * 4);
			if (!m_callback.IsNull())
			{
				m_callback(bits, lsize);
			}
		}
	}

	BOOL MFLVPlayer::Close()
	{
		m_bPlaying = FALSE;
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
		m_clock.SetBasePTS(-1);
		m_clock.SetBaseTime(-1);
		Sleep(100);
		return bRes;
	}

	TinySize MFLVPlayer::GetVideoSize()
	{
		return m_size;
	}
}

