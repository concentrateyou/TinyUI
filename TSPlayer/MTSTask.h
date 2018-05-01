#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "MClock.h"
#include "TSReader.h"
#include "Network/TinyHTTPClient.h"
using namespace TinyUI::Network;

namespace TSPlayer
{
	/// <summary>
	/// TS∂¡»°œﬂ≥Ã
	/// </summary>
	class MTSTask : public TinyThread
	{
		DISALLOW_COPY_AND_ASSIGN(MTSTask)
	public:
		MTSTask(MClock& clock, TinyMsgQueue& queue);
		virtual ~MTSTask();
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
		void			OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID);
		BOOL			Invoke(SampleTag& tag, TS_BLOCK& block);
	private:
		BOOL			m_bBreak;
		LONGLONG		m_sample;
		TinyMsgQueue&	m_msgqueue;
		MClock&			m_clock;
		MPacketQueue	m_audioQueue;
		MPacketQueue	m_videoQueue;
		TSReader		m_reader;
	};
}


