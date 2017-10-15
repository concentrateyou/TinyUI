#pragma once
#include "MRTMPClient.h"
#include "x264Encode.h"
#include <concurrent_queue.h>

namespace MShow
{
	class MRTMPEncoder;
	/// <summary>
	/// RTMPÍÆÁ÷
	/// </summary>
	class MRTMPPusher : public TinyTaskBase
	{
		friend class MRTMPEncoder;
		DISALLOW_COPY_AND_ASSIGN(MRTMPPusher)
	public:
		MRTMPPusher();
		virtual ~MRTMPPusher();
		BOOL Connect(const TinyString& szURL = "rtmp://101.227.14.238:1935/liveugc/rqy_oi4wd5ka?auth_code=pt14yy4g", DWORD dwRate = 25);
		BOOL Disconnect();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		TinyString GetURL() const;
	private:
		void OnMessagePump();
		void Publish(Sample& sample);
	private:
		BOOL			m_bClose;
		DWORD			m_dwRate;
		DWORD			m_dwReconnect;
		MRTMPClient		m_client;
		TinyString		m_szURL;
	};
}

