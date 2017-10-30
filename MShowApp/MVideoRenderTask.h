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
		MVideoRenderTask(MVideoTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& copyCB, Closure&& renderCB);
		virtual ~MVideoRenderTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL		m_bBreak;
		MClock&		m_clock;
		MVideoTask& m_task;
		Closure		m_renderCB;
		TinyUI::Callback<void(BYTE*, LONG)> m_copyCB;
		TinyPerformanceTime m_timeQPC;
	};
}


