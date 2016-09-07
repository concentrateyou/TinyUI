#include "stdafx.h"
#include "RTMPPublisher.h"
#include "x264Encode.h"

#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
#define SAVC(x)    static const AVal av_##x = AVC(#x)
static const AVal av_setDataFrame = AVC("@setDataFrame");
SAVC(onMetaData);
SAVC(duration);
SAVC(width);
SAVC(height);
SAVC(videocodecid);
SAVC(videodatarate);
SAVC(framerate);
SAVC(audiocodecid);
SAVC(audiodatarate);
SAVC(audiosamplerate);
SAVC(audiosamplesize);
SAVC(audiochannels);
SAVC(stereo);
SAVC(encoder);
SAVC(av_stereo);
SAVC(avc1);
SAVC(fileSize);

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
	if (!RTMP_Connect(m_pRTMP, NULL))
		return FALSE;
	if (!RTMP_ConnectStream(m_pRTMP, 0))
		return FALSE;
	return TRUE;
}

BOOL RTMPPublisher::SendMetadata(INT cx, INT cy, INT fps, INT rate)
{
	ASSERT(m_pRTMP);
	if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
	{
		return FALSE;
	}
	RTMPPacket* packet = NULL;
	CHAR* body = NULL;
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE + 1024);
	memset(packet, 0, RTMP_HEAD_SIZE);
	packet->m_body = (CHAR*)packet + RTMP_HEAD_SIZE;
	body = (CHAR*)packet->m_body;
	CHAR* ebody = body + 1024;
	body = AMF_EncodeString(body, ebody, &av_setDataFrame);
	body = AMF_EncodeString(body, ebody, &av_onMetaData);
	*body++ = AMF_OBJECT;
	body = AMF_EncodeNamedNumber(body, ebody, &av_duration, 0.0);
	body = AMF_EncodeNamedNumber(body, ebody, &av_fileSize, 0.0);
	body = AMF_EncodeNamedNumber(body, ebody, &av_width, static_cast<double>(cx));
	body = AMF_EncodeNamedNumber(body, ebody, &av_height, static_cast<double>(cy));
	body = AMF_EncodeNamedString(body, ebody, &av_videocodecid, &av_avc1);
	body = AMF_EncodeNamedNumber(body, ebody, &av_videodatarate, static_cast<double>(rate));
	body = AMF_EncodeNamedNumber(body, ebody, &av_framerate, static_cast<double>(fps));
	*body++ = 0;
	*body++ = 0;
	*body++ = AMF_OBJECT_END;
	packet->m_nBodySize = body - packet->m_body;
	packet->m_packetType = RTMP_PACKET_TYPE_INFO;
	packet->m_nChannel = 0x03;
	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet->m_nTimeStamp = 0;
	packet->m_hasAbsTimestamp = 0;
	packet->m_nInfoField2 = m_pRTMP->m_stream_id;
	RTMP_SendPacket(m_pRTMP, packet, TRUE);
	SAFE_FREE(packet);
	return TRUE;
}
BOOL RTMPPublisher::SendSPSPPS(const vector<BYTE>& pps, const vector<BYTE>& sps)
{
	ASSERT(m_pRTMP);
	if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
	{
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
BOOL RTMPPublisher::SendAudioRTMP(BYTE* data, INT size, DWORD timeoffset)
{
	if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
	{
		return FALSE;
	}
	RTMPPacket* packet = NULL;
	BYTE* body = NULL;
	if (data[2] == 0x00)
	{
		data += 4;
		size -= 4;
	}
	if (data[2] == 0x01)
	{
		data += 3;
		size -= 3;
	}
	INT type = data[0] & 0x1f;
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
	body[8] = (size)& 0xFF;
	memcpy(&body[9], data, size);
	packet->m_hasAbsTimestamp = 0;
	packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
	packet->m_nInfoField2 = m_pRTMP->m_stream_id;
	packet->m_nChannel = 0x05;
	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet->m_nTimeStamp = timeoffset;
	RTMP_SendPacket(m_pRTMP, packet, TRUE);
	SAFE_FREE(packet);
	return TRUE;
	return TRUE;
}
BOOL RTMPPublisher::SendVideoRTMP(BYTE* data, INT size, DWORD timeoffset)
{
	if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
	{
		return FALSE;
	}
	RTMPPacket* packet = NULL;
	BYTE* body = NULL;
	if (data[2] == 0x00)
	{
		data += 4;
		size -= 4;
	}
	if (data[2] == 0x01)
	{
		data += 3;
		size -= 3;
	}
	INT type = data[0] & 0x1f;
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
	body[8] = (size)& 0xFF;
	memcpy(&body[9], data, size);
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
