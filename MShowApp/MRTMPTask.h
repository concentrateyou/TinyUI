#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "MClock.h"
#include "FLVReader.h"

namespace MShow
{
	/// <summary>
	/// RTMP∂¡»°œﬂ≥Ã
	/// </summary>
	class MRTMPTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MRTMPTask)
	public:
		MRTMPTask();
		virtual ~MRTMPTask();
		BOOL Initialize(LPCSTR pzURL);
		BOOL Submit();
		BOOL Close();
		MPacketQueue&	GetAudioQueue();
		MPacketQueue&	GetVideoQueue();
		FLV_SCRIPTDATA&	GetScript();
	public:
		Event<void(BYTE*, LONG, WORD)>	EVENT_ASC;
		Event<void(BYTE*, LONG)>		EVENT_AVCDCR;
	private:
		void OnMessagePump();
	private:
		BOOL			m_bFI;
		BOOL			m_bClose;
		LONGLONG		m_sample;
		FLVReader		m_reader;
		FLV_SCRIPTDATA	m_script;
		MClock			m_clock;
		MPacketQueue	m_audioQueue;
		MPacketQueue	m_videoQueue;
	};

}


