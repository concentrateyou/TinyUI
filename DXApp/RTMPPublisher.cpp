#include "stdafx.h"
#include "RTMPPublisher.h"
#include "x264Encode.h"

#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)

RTMPPublisher::RTMPPublisher()
	:m_pRTMP(NULL)
{

}


RTMPPublisher::~RTMPPublisher()
{
	if (m_pRTMP)
	{
		RTMP_Close(m_pRTMP);
		RTMP_Free(m_pRTMP);
	}
}

BOOL RTMPPublisher::Connect(const TinyString& url)
{
	if (m_pRTMP)
	{
		RTMP_Close(m_pRTMP);
		RTMP_Free(m_pRTMP);
	}
	m_pRTMP = RTMP_Alloc();
	RTMP_Init(m_pRTMP);
	if (!RTMP_SetupURL(m_pRTMP, url.STR()))
		return FALSE;
	RTMP_EnableWrite(m_pRTMP);
	INT iRes = RTMP_Connect(m_pRTMP, NULL);
	if (!iRes)
		return FALSE;
	iRes = RTMP_ConnectStream(m_pRTMP, 0);
	if (!iRes)
		return FALSE;
	m_dwStart = GetTickCount();
	return TRUE;
}

BOOL RTMPPublisher::SendSPSPPS(const vector<BYTE>& pps, const vector<BYTE>& sps)
{
	ASSERT(m_pRTMP);
	if (!RTMP_IsConnected(m_pRTMP))
	{
		TRACE("SendVideoRTMP Disconnect\n");
		return FALSE;
	}
	if (RTMP_IsTimedout(m_pRTMP))
	{
		TRACE("SendVideoRTMP Timeout\n");
		return FALSE;
	}
	RTMPPacket* packet = NULL;
	BYTE* body = NULL;
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE + 1024);
	memset(packet, 0, RTMP_HEAD_SIZE);
	packet->m_body = (CHAR*)packet + RTMP_HEAD_SIZE;
	body = (BYTE*)packet->m_body;
	INT  i = 0;
	body[i++] = 0x17;
	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x01;
	body[i++] = sps[1];
	body[i++] = sps[2];
	body[i++] = sps[3];
	body[i++] = 0xff;
	body[i++] = 0xe1;
	body[i++] = (sps.size() >> 8) & 0xFF;
	body[i++] = sps.size() & 0xFF;
	memcpy(&body[i], &sps[0], sps.size());
	i += sps.size();
	body[i++] = 0x01;
	body[i++] = (pps.size() >> 8) & 0xFF;
	body[i++] = (pps.size()) & 0xFF;
	memcpy(&body[i], &pps[0], pps.size());
	i += pps.size();
	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nBodySize = i;
	packet->m_nChannel = 0x04;
	packet->m_nTimeStamp = 0;
	packet->m_hasAbsTimestamp = 0;
	packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet->m_nInfoField2 = m_pRTMP->m_stream_id;
	RTMP_SendPacket(m_pRTMP, packet, TRUE);
	SAFE_FREE(packet);
	return TRUE;
}

BOOL RTMPPublisher::SendVideoRTMP(BYTE* pBits, INT size)
{
	if (!RTMP_IsConnected(m_pRTMP))
	{
		TRACE("SendVideoRTMP Disconnect\n");
		return FALSE;
	}
	if (RTMP_IsTimedout(m_pRTMP))
	{
		TRACE("SendVideoRTMP Timeout\n");
		return FALSE;
	}
	DWORD timeoffset = 0;
	RTMPPacket* packet = NULL;
	BYTE* body = NULL;

	timeoffset = GetTickCount() - m_dwStart;
	if (pBits[2] == 0x00)
	{
		pBits += 4;
		size -= 4;
	}
	else if (pBits[2] == 0x01)
	{
		pBits += 3;
		size -= 3;
	}
	INT type = pBits[0] & 0x1f;
	packet = (RTMPPacket*)malloc(RTMP_HEAD_SIZE + size + 9);
	memset(packet, 0, RTMP_HEAD_SIZE);
	packet->m_body = (CHAR*)packet + RTMP_HEAD_SIZE;
	packet->m_nBodySize = size + 9;
	body = (BYTE*)packet->m_body;
	memset(body, 0, size + 9);
	body[0] = 0x27;
	if (type == NAL_SLICE_IDR)
	{
		body[0] = 0x17;
	}
	body[1] = 0x01;
	body[2] = 0x00;
	body[3] = 0x00;
	body[4] = 0x00;
	body[5] = (size >> 24) & 0xFF;
	body[6] = (size >> 16) & 0xFF;
	body[7] = (size >> 8) & 0xFF;
	body[8] = (size)& 0xff;
	memcpy(&body[9], pBits, size);
	packet->m_hasAbsTimestamp = 0;
	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nInfoField2 = m_pRTMP->m_stream_id;
	packet->m_nChannel = 0x04;
	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet->m_nTimeStamp = timeoffset;
	RTMP_SendPacket(m_pRTMP, packet, TRUE);
	SAFE_FREE(packet);
	return TRUE;
}

BOOL RTMPPublisher::Reconnect()
{
	ASSERT(m_pRTMP);
	if (!RTMP_ReconnectStream(m_pRTMP, 0))
		return FALSE;
	return TRUE;
}
