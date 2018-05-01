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
	class MAudioRenderTask : public TinyThread
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioRenderTask)
	public:
		MAudioRenderTask(MAudioTask& task, MClock& clock);
		MAudioRenderTask(MAudioTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MAudioRenderTask();
		BOOL Initialize();
		BOOL SetVolume(DWORD dwVolume);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL					m_bBreak;
		BOOL					m_bInitialize;
		MClock&					m_clock;
		MAudioTask&				m_task;
		TinyXAudio				m_audio;
		TinyUI::Callback<void(BYTE*, LONG)> m_callback;
	};
}


