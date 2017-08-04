#pragma once
#include "MRTMPClient.h"
#include "x264Encode.h"
#include <concurrent_queue.h>

namespace MShow
{
	class MRTMPEncoder;
	/// <summary>
	/// 排序的采样队列
	/// </summary>
	class MSampleQueue
	{
		DISALLOW_COPY_AND_ASSIGN(MSampleQueue)
	public:
		MSampleQueue();
		~MSampleQueue();
	public:
		BOOL Push(Sample& sample);
		BOOL Pop(Sample& sample);
	private:
		void Print();
	private:
		TinyLock				m_lock;
		TinyLinkList<Sample>	m_samples;
	};

	/// <summary>
	/// RTMP推流
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
		TinyString GetURL() const;
		void Publish(Sample& sample);
		void Add(Sample& sample);
	private:
		void OnMessagePump();
	private:
		BOOL			m_bClose;
		DWORD			m_dwRate;
		DWORD			m_dwReconnect;
		MRTMPClient		m_client;
		TinyString		m_szURL;
		MSampleQueue	m_samples;
	};
}

