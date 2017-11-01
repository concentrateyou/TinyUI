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
		MVideoRenderTask(MVideoTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& copyCB, Closure&& renderCB);
		virtual ~MVideoRenderTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL		m_bBreak;
		Closure		m_renderCB;
		MClock&		m_clock;
		MVideoTask& m_task;
		TinyPerformanceTime m_timeQPC;
		TinyUI::Callback<void(BYTE*, LONG)> m_copyCB;
	};
}


