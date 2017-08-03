#pragma once
#include "MShowCommon.h"
#include "AudioCapture.h"
#include "MPacketQueue.h"
#include "MRTMPPusher.h"

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
		MAudioEncodeTask(MRTMPPusher& pusher);
		virtual ~MAudioEncodeTask();
	public:
		INT				GetAudioRate() const;
		WAVEFORMATEX	GetFormat() const;
		BOOL			Submit(INT audioRate = 128);
		BOOL			Close(DWORD dwMS) OVERRIDE;
		AACEncode&		GetAAC();
		void			SetVideoController(MVideoController* pCTRL);
		void			SetAudioController(MAudioController* pCTRL);
	private:
		void OnMessagePump();
		void OnAudio(BYTE* bits, LONG size);
		void OnAudio1(BYTE* bits, LONG size);
		void OnAAC(BYTE*, LONG, const MediaTag&);
	private:
		BOOL					m_bBreak;
		INT						m_audioRate;
		MRTMPPusher&			m_pusher;
		WAVEFORMATEX			m_waveFMT;
		AACEncode				m_aac;
		MPacketQueue			m_queue;
		MPacketQueue			m_queue1;
		MVideoController*		m_pVideoCTRL;
		MAudioController*		m_pAudioCTRL;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>> m_onAudio;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>> m_onEffectAudio;
	};
}
