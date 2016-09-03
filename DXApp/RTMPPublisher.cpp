#include "stdafx.h"
#include "RTMPPublisher.h"


RTMPPublisher::RTMPPublisher()
	:m_pRTMP(NULL)
{
	m_pRTMP = RTMP_Alloc();
	RTMP_Init(m_pRTMP);
	RTMP_LogSetLevel(RTMP_LOGINFO);
	RTMP_LogSetOutput(stderr);
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
	ASSERT(m_pRTMP);
	if (!RTMP_SetupURL(m_pRTMP, url.STR()))
		return FALSE;
	m_pRTMP->Link.lFlags |= RTMP_LF_BUFX;
	RTMP_EnableWrite(m_pRTMP);
	if (!RTMP_Connect(m_pRTMP, NULL))
		return FALSE;
	if (!RTMP_ConnectStream(m_pRTMP, 0))
		return FALSE;
	return TRUE;
}

BOOL RTMPPublisher::Write(BYTE* pBits, INT size)
{
	ASSERT(m_pRTMP);
	if (!RTMP_IsConnected(m_pRTMP))
		return FALSE;
	if (RTMP_IsTimedout(m_pRTMP))
		return FALSE;
	INT send_size = RTMP_Write(m_pRTMP, reinterpret_cast<const CHAR*>(pBits), size);
	return send_size == size;
}

BOOL RTMPPublisher::Reconnect()
{
	ASSERT(m_pRTMP);
	if (!RTMP_ReconnectStream(m_pRTMP, 0))
		return FALSE;
	return TRUE;
}
