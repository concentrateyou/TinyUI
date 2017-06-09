#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MReadTask.h"

namespace MShow
{
	/// <summary>
	/// “Ù∆µΩ‚¬Îœﬂ≥Ã
	/// </summary>
	class MAudioTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MReadTask& task,MClock& clock);
		virtual ~MAudioTask();
		BOOL Close(DWORD dwMS) OVERRIDE;
		MPacketQueue& GetQueue();
		TinyLock&	GetLock();
		AACDecode*	GetAAC();
	private:
		void OnMessagePump();
	private:
		BOOL			m_close;
		TinyLock		m_lock;
		MClock&			m_clock;
		MReadTask&		m_task;
		MPacketQueue	m_audioQueue;
	};
}



