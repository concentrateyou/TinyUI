#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "MClock.h"
#include "HLSReader.h"
#include "Network/TinyHTTPClient.h"
using namespace TinyUI::Network;

namespace HLSPlayer
{
	/// <summary>
	/// HLS��ȡ�߳�
	/// </summary>
	class MHLSTask : public TinyThread
	{
		DISALLOW_COPY_AND_ASSIGN(MHLSTask)
	public:
		MHLSTask(MClock& clock, TinyMsgQueue& queue);
		virtual ~MHLSTask();
		BOOL			Initialize(LPCSTR pzURL, ErrorCallback&& callback);
		BOOL			Submit();
		BOOL			Close(DWORD dwMS) OVERRIDE;
		MPacketQueue&	GetAudioQueue();
		MPacketQueue&	GetVideoQueue();
		TinySize		GetVideoSize();
	public:
		Event<void(BYTE*, LONG, WORD, BOOL&)>	EVENT_ASC;
		Event<void(BYTE*, LONG, BOOL&)>			EVENT_AVCDCR;
	private:
		void			OnMessagePump();
		void			OnSegments();
		void			OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID);
		BOOL			Invoke(SampleTag& tag, TS_BLOCK& block);
	private:
		BOOL				m_bBreak;
		LONGLONG			m_sample;
		HLSReader::Segment	m_segment;
		TinyMsgQueue&		m_msgqueue;
		MClock&				m_clock;
		MPacketQueue		m_audioQueue;
		MPacketQueue		m_videoQueue;
		TinyHTTPClient		m_client;
		HLSReader			m_readerHLS;
		TSReader			m_readerTS;
		TinySimpleTaskTimer	m_timer;
	};
}


