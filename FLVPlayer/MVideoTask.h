#pragma once
#include "MFLVTask.h"
#include "QSVDecoder.h"
using namespace QSV;

namespace FLVPlayer
{
	/// <summary>
	/// ��Ƶ�����߳�
	/// </summary>
	class MVideoTask : public TinyThread
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
		QSVDecoder					m_qsv;
		//x264Decoder					m_x264;
		TinyMsgQueue&				m_msgqueue;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketQueue				m_videoQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, BOOL&)>>	m_onAVCDC;
	};
}