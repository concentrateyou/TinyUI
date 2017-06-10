#pragma once
#include "MShowCommon.h"
#include "MReadTask.h"
#include "MClock.h"

namespace MShow
{
	/// <summary>
	/// 视频解码线程
	/// </summary>
	class MVideoTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask(MReadTask& task, MClock& clock);
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
		MReadTask&		m_task;
		MPacketQueue	m_videoQueue;
	};
}