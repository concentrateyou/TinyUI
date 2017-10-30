#pragma once
#include "MShowCommon.h"
#include "AudioCapture.h"
#include "MPacketQueue.h"
#include "MSampleQueue.h"
#include "MRTMPPusher.h"
#include "MClock.h"
using namespace DShow;

namespace MShow
{
	class MVideoController;
	class MAudioController;
	/// <summary>
	/// 采集声卡数据和音频编码
	/// </summary>
	class MAudioEncodeTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioEncodeTask)
	public:
		MAudioEncodeTask(MRTMPPusher& pusher, MClock& clock);
		virtual ~MAudioEncodeTask();
	public:
		INT				GetAudioRate() const;
		WAVEFORMATEX	GetFormat() const;
		BOOL			Submit(INT audioRate = 128);
		BOOL			Close(DWORD dwMS) OVERRIDE;
		AACEncode&		GetAAC();
		void			SetVideoController(MVideoController* pCTRL);
		void			SetAudioController(MAudioController* pCTRL);
		MSampleQueue&	GetSamples();
	private:
		void OnMessagePump();
		void OnAudio(BYTE* bits, LONG size);
		void OnAudioMix(BYTE* bits, LONG size);
	private:
		BOOL					m_bBreak;
		INT						m_audioRate;
		MClock&					m_clock;
		MRTMPPusher&			m_pusher;
		WAVEFORMATEX			m_waveFMT;
		AACEncode				m_aac;
		MPacketQueue			m_queue;
		MPacketQueue			m_queueMix;
		MSampleQueue			m_samples;
		MVideoController*		m_pVideoCTRL;
		MAudioController*		m_pAudioCTRL;
		TinyPerformanceTime		m_timeQPC;
		TinyPerformanceTimer	m_timer;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>> m_onAudio;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>> m_onAudioMix;
	};
}
