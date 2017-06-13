#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MFLVScene.h"

namespace MShow
{
	class MFLVScene;
	/// <summary>
	/// “Ù∆µΩ‚¬Îœﬂ≥Ã
	/// </summary>
	class MAudioTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MFLVScene& task, MClock& clock);
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
		MFLVScene&		m_task;
		MPacketQueue	m_queue;
	};
}



