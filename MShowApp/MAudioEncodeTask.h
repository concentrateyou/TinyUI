#pragma once
#include "MShowCommon.h"
#include "AudioCapture.h"
#include "MRTMPPusher.h"
using namespace DShow;

namespace MShow
{
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
	private:
		void OnMessagePump();
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
		void OnAAC(BYTE*, LONG, const MediaTag&);
	private:
		MRTMPPusher&			m_pusher;
		BOOL					m_bBreak;
		INT						m_audioRate;
		WAVEFORMATEX			m_waveFMT;
		AudioCapture			m_audio;
		AACEncode				m_aac;
	};
}
