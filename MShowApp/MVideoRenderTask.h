#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"

namespace MShow
{
	class MVideoTask;
	/// <summary>
	/// ��Ƶ�����߳�
	/// </summary>
	class MVideoRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoRenderTask)
	public:
		MVideoRenderTask(MVideoTask& task, MClock& clock, DX2D& d2d);
		virtual ~MVideoRenderTask();
		BOOL Initialize();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
		void OnRender(BYTE* bits, LONG size);
	private:
		DX2D&		m_d2d;
		TinySize	m_size;
		TinyEvent	m_close;
		MClock&		m_clock;
		MVideoTask& m_task;
		TinyComPtr<ID2D1Bitmap1> m_bitmap;
	};
}


