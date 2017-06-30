#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"

namespace MShow
{
	class MAudioTask;
	/// <summary>
	/// ≤•∑≈“Ù∆µœﬂ≥Ã
	/// </summary>
	class MAudioRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioRenderTask)
	public:
		MAudioRenderTask(MAudioTask& task, MClock& clock);
		MAudioRenderTask(MAudioTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MAudioRenderTask();
		BOOL Initialize(HWND hWND);
		BOOL SetVolume(LONG volume);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL					m_bBreak;
		BOOL					m_bInitialize;
		TinyEvent				m_events[3];
		MClock&					m_clock;
		MAudioTask&				m_task;
		TinySoundPlayer			m_player;
		TinyUI::Callback<void(BYTE*, LONG)> m_callback;
	};
}


