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
		MFLVPlayer(DX2D& dx2d, Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MFLVPlayer();
		BOOL IsPlaying() const;
		BOOL Open(LPCSTR pzURL);
		BOOL Close();
		BOOL SetVolume(LONG volume);
		TinySize GetVideoSize();
		ID2D1Bitmap1* GetBitmap();
	private:
		void OnVideo(BYTE* bits, LONG size);
	private:
		BOOL							m_bPlaying;
		DX2D&							m_dx2d;
		TinySize						m_size;
		MClock							m_clock;
		MFLVTask						m_task;
		MAudioTask						m_audioTask;
		MVideoTask						m_videoTask;
		MAudioRenderTask				m_audioRenderTask;
		MVideoRenderTask				m_videoRenderTask;
		TinyComPtr<ID2D1Bitmap1>		m_bitmap;
		Callback<void(BYTE*, LONG)>		m_callback;
	};
}


