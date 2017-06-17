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
	class MAudioTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MFLVTask& task, MClock& clock);
		virtual ~MAudioTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		WAVEFORMATEX* GetFormat();
	public:
		MPacketQueue&	GetAudioQueue();
	private:
		void OnMessagePump();
		void OnASC(BYTE* bits, LONG size, WORD wBitsPerSample);
	private:
		BOOL						m_bClose;
		AACDecode					m_aac;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketQueue				m_audioQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, WORD)>>	m_onASC;
	};
}



