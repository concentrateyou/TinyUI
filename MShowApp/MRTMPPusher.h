#pragma once
#include "MRTMPClient.h"

namespace MShow
{
	/// <summary>
	/// RTMPÍÆÁ÷
	/// </summary>
	class MRTMPPusher : public TinyTaskBase
	{
	public:
		MRTMPPusher();
		virtual ~MRTMPPusher();
		BOOL Connect(const TinyString& szURL = "rtmp://10.121.86.127/live/test", DWORD dwRate = 25);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL			m_bClose;
		DWORD			m_dwRate;
		DWORD			m_dwReconnect;
		MRTMPClient		m_client;
	};
}

