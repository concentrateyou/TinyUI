#pragma once
#include "MRTMPTask.h"

namespace MShow
{
	class MFLVScene;
	/// <summary>
	/// 视频解码线程
	/// </summary>
	class MVideoTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask(MRTMPTask& task, MClock& clock);
		virtual ~MVideoTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		MPacketQueue& GetVideoQueue();
	private:
		void OnMessagePump();
		void OnAVCDC(BYTE* bits, LONG size);
	private:
		BOOL						m_bClose;
		MClock&						m_clock;
		MRTMPTask&					m_task;
		MPacketQueue				m_videoQueue;
		H264Decode					m_h264;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>>	m_onAVCDC;
	};
}