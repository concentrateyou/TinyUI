#pragma once
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	/// <summary>
	/// FLV²¥·ÅÆ÷
	/// </summary>
	class MFLVPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVPlayer)
	public:
		MFLVPlayer(DX2D& d2d);
		virtual ~MFLVPlayer();
		BOOL Open(LPCSTR pzURL);
		BOOL Close();
		ID2D1Bitmap1* GetBitmap();
	private:
		void OnVideo(BYTE* bits, LONG size);
	private:
		DX2D&						m_d2d;
		TinySize					m_size;
		MClock						m_clock;
		MFLVTask					m_task;
		MAudioTask					m_audioTask;
		MVideoTask					m_videoTask;
		MAudioRenderTask			m_audioRenderTask;
		MVideoRenderTask			m_videoRenderTask;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap;
	};
}


