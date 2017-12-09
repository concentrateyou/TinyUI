#pragma once
#include "MFLVTask.h"
#include "QSVDecoder.h"

namespace MShow
{
	/// <summary>
	/// 视频解码线程
	/// </summary>
	class MVideoTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask(MFLVTask& task, MClock& clock, TinyMsgQueue& queue);
		virtual ~MVideoTask();
		BOOL			Submit();
		BOOL			Close(DWORD dwMS) OVERRIDE;
		TinySize		GetVideoSize();
		MPacketQueue&	GetVideoQueue();
	private:
		void OnMessagePump();
		void OnAVCDC(BYTE* bits, LONG size, BOOL&);
	private:
		BOOL						m_bBreak;
		x264Decode					m_x264;
		TinyMsgQueue&				m_msgqueue;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketQueue				m_videoQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, BOOL&)>>	m_onAVCDC;
	};
}