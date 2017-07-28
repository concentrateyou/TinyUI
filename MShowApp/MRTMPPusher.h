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
		BOOL Connect(const TinyString& szURL = "rtmp://10.121.86.127/live/test_mshow", DWORD dwRate = 25);
		BOOL Disconnect();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		void Publish(Sample& sample);
	private:
		void OnMessagePump();
	private:
		BOOL			m_bClose;
		DWORD			m_dwRate;
		DWORD			m_dwReconnect;
		MRTMPClient		m_client;
		vector<BYTE>	m_latestPPS;
		vector<BYTE>	m_latestSPS;
		Concurrency::concurrent_queue<Sample>	m_samples;
	};
}

