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

void RTMPPublisher::SetSPS(BYTE* sps, DWORD dwSPS)
{
	m_dwSPS = dwSPS;
	memcpy(m_sps, sps, m_dwSPS);
}
void RTMPPublisher::SetPPS(BYTE* pps, DWORD dwPPS)
{
	m_dwPPS = dwPPS;
	memcpy(m_pps, pps, m_dwSPS);
}

BOOL RTMPPublisher::SendSPSPPS()
{
	ASSERT(m_pRTMP);
	if (!RTMP_IsConnected(m_pRTMP))
		return FALSE;
	if (RTMP_IsTimedout(m_pRTMP))
		return FALSE;
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
	body[i++] = m_sps[1];
	body[i++] = m_sps[2];
	body[i++] = m_sps[3];
	body[i++] = 0xff;
	body[i++] = 0xe1;
	body[i++] = (m_dwSPS >> 8) & 0xff;
	body[i++] = m_dwSPS & 0xff;
	memcpy(&body[i], m_sps, m_dwSPS);
	i += m_dwSPS;
	body[i++] = 0x01;
	body[i++] = (m_dwPPS >> 8) & 0xff;
	body[i++] = (m_dwPPS)& 0xff;
	memcpy(&body[i], m_pps, m_dwPPS);
	i += m_dwPPS;
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

BOOL RTMPPublisher::SendVideo(BYTE* pBits, INT size)
{
	if (!RTMP_IsConnected(m_pRTMP))
		return FALSE;
	if (RTMP_IsTimedout(m_pRTMP))
		return FALSE;
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
	body[5] = (size >> 24) & 0xff;
	body[6] = (size >> 16) & 0xff;
	body[7] = (size >> 8) & 0xff;
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
