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
		MVideoRenderTask(MVideoTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MVideoRenderTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL		m_bBreak;
		MClock&		m_clock;
		MVideoTask& m_task;
		TinyUI::Callback<void(BYTE*, LONG)> m_callback;
	};
}


