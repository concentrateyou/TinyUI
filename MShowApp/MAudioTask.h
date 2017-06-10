#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MFLVTask.h"

namespace MShow
{
	class MFLVTask;
	/// <summary>
	/// “Ù∆µΩ‚¬Îœﬂ≥Ã
	/// </summary>
	class MAudioTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MFLVTask& task, MClock& clock);
		virtual ~MAudioTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		MPacketQueue&	GetQueue();
		AACDecode*		GetAAC();
	private:
		void OnMessagePump();
	private:
		TinyEvent		m_close;
		MClock&			m_clock;
		MFLVTask&		m_task;
		MPacketQueue	m_queue;
	};
}



