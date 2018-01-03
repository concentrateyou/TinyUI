#pragma once
#include "MFLVTask.h"
#include "QSVDecoder.h"

namespace MShow
{
	/// <summary>
	/// ��Ƶ�����߳�
	/// </summary>
	class MVideoTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask(MFLVTask& task, MClock& clock);
		virtual ~MVideoTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		MPacketAllocQueue& GetVideoQueue();
	private:
		void OnMessagePump();
		void OnAVCDC(BYTE* bits, LONG size, BOOL&);
	private:
		FILE*						m_hFile;
		BOOL						m_bBreak;
		x264Decoder					m_x264;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketAllocQueue			m_videoQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, BOOL&)>>	m_onAVCDC;
	};
}