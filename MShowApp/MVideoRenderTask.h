#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"
#include "DXView.h"

namespace MShow
{
	/// <summary>
	/// 视频播放线程
	/// </summary>
	class MVideoRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoRenderTask)
	public:
		MVideoRenderTask(MVideoTask& task,MClock& clock);
		virtual ~MVideoRenderTask();
		BOOL Initialize(HWND hWND);
	private:
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL		m_close;
		DX2D		m_d2d;
		MClock&		m_clock;
		MVideoTask& m_task;		
	};
}


