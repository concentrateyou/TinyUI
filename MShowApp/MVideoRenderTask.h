#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"

namespace MShow
{
	class MVideoTask;
	/// <summary>
	/// 视频播放线程
	/// </summary>
	class MVideoRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoRenderTask)
	public:
		MVideoRenderTask(MVideoTask& task, MClock& clock, DX2D& d2d, TinyUI::Callback<void(ID2D1Bitmap1*, INT)>&& callback);
		virtual ~MVideoRenderTask();
		BOOL Initialize();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		ID2D1Bitmap1* GetBitmap();
	private:
		void OnMessagePump();
		void OnRender(BYTE* bits, LONG size,INT delay);
	private:
		DX2D&		m_d2d;
		TinySize	m_size;
		TinyEvent	m_close;
		MClock&		m_clock;
		MVideoTask& m_task;
		TinyComPtr<ID2D1Bitmap1> m_bitmap;
		TinyUI::Callback<void(ID2D1Bitmap1*, INT)> m_callback;
	};
}


