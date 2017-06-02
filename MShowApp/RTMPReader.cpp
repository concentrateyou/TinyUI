#include "stdafx.h"
#include "RTMPReader.h"

namespace MShow
{
	RTMPReader::RTMPReader()
	{
	}

	RTMPReader::~RTMPReader()
	{
	}

	BOOL RTMPReader::Open(LPCSTR pzURL)
	{
		RTMP_Init(&m_sRTMP);
		if (!RTMP_SetupURL(&m_sRTMP, (CHAR*)pzURL))
			return FALSE;
		m_sRTMP.Link.timeout = 3000;//默认3秒超时
		m_sRTMP.Link.lFlags |= RTMP_LF_BUFX | RTMP_LF_FTCU | RTMP_LF_LIVE;
		RTMP_SetBufferMS(&m_sRTMP, 3600 * 1000);//1h
		if (!RTMP_Connect(&m_sRTMP, NULL))
			return FALSE;
		if (!RTMP_ConnectStream(&m_sRTMP, 0))
			return FALSE;
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&RTMPReader::OnMessagePump, this));
	}

	BOOL RTMPReader::Close(DWORD dwMs)
	{
		RTMP_Close(&m_sRTMP);
		return TinyTaskBase::Close(dwMs);
	}

	void RTMPReader::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			if (!RTMP_IsConnected(&m_sRTMP) ||
				RTMP_IsTimedout(&m_sRTMP))
				break;
			RTMPPacket packet = { 0 };
			INT val = RTMP_GetNextMediaPacket(&m_sRTMP, &packet);
			if (val <= 0 || val == 2)
				break;
			CHAR *packetBody = packet.m_body;
			UINT bodySize = packet.m_nBodySize;
			if (packet.m_packetType == 0x09)//音频
			{
				
			}
			if (packet.m_packetType == 0x08)//视频
			{
				
			}
			if (packet.m_packetType == 0x12)//信息
			{
				
			}
			RTMPPacket_Free(&packet);
		}
	}
}