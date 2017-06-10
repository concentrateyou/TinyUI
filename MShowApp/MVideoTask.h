#pragma once
#include "MShowCommon.h"
#include "MFLVTask.h"
#include "MClock.h"

namespace MShow
{
	class MFLVTask;
	/// <summary>
	/// 视频解码线程
	/// </summary>
	class MVideoTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask(MFLVTask& task, MClock& clock);
		virtual ~MVideoTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		MPacketQueue&	GetQueue();
		H264Decode*		GetH264();
		TinySize		GetSize() const;
	private:
		void OnMessagePump();
	private:
		TinyEvent		m_close;
		MClock&			m_clock;
		MFLVTask&		m_task;
		MPacketQueue	m_queue;
	};
}