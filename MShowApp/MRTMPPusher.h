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
		DWORD					m_dwRate;
		DWORD					m_dwReconnect;
		BOOL					m_bClose;
		MRTMPClient				m_client;
	};
}

