#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MFLVTask.h"

namespace MShow
{
	class MFLVModel;
	/// <summary>
	/// “Ù∆µΩ‚¬Îœﬂ≥Ã
	/// </summary>
	class MAudioTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MFLVTask& task, MClock& clock, TinyMsgQueue& queue);
		virtual ~MAudioTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		WAVEFORMATEX*	GetFormat();
		LONGLONG		GetBasePTS();
	public:
		MPacketQueue&	GetAudioQueue();
	private:
		void OnMessagePump();
		void OnASC(BYTE* bits, LONG size, WORD wBitsPerSample, BOOL& bRes);
	private:
		BOOL						m_bBreak;
		AACDecoder					m_aac;
		TinyMsgQueue&				m_msgqueue;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketQueue				m_audioQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, WORD, BOOL&)>>	m_onASC;
	};
}



