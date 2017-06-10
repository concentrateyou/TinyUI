#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"

namespace MShow
{
	/// <summary>
	/// ≤•∑≈“Ù∆µœﬂ≥Ã
	/// </summary>
	class MAudioRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioRenderTask)
	public:
		MAudioRenderTask(MAudioTask& task, MClock& clock);
		virtual ~MAudioRenderTask();
		BOOL Initialize(HWND hWND);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL					m_bInitialize;
		TinyEvent				m_close;
		TinyEvent				m_events[2];
		MClock&					m_clock;
		MAudioTask&				m_task;
		TinySoundPlayer			m_player;
		TinyPerformanceTimer	m_timer;
	};
}


