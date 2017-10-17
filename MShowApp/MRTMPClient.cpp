#include "stdafx.h"
#include "MRTMPClient.h"
#include "x264Encode.h"

namespace MShow
{
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
	SAVC(mp4a);

	MRTMPClient::MRTMPClient()
		:m_pRTMP(NULL)
	{

	}

	MRTMPClient::~MRTMPClient()
	{
		if (m_pRTMP != NULL)
		{
			RTMP_Close(m_pRTMP);
			RTMP_Free(m_pRTMP);
			m_pRTMP = NULL;
		}
	}

	DWORD MRTMPClient::GetTime()
	{
		return RTMP_GetTime();
	}

	BOOL MRTMPClient::Connect(const TinyString& szURL)
	{
		/*if (m_pRTMP != NULL)
		{
			RTMP_Close(m_pRTMP);
			RTMP_Free(m_pRTMP);
			m_pRTMP = NULL;
		}
		m_pRTMP = RTMP_Alloc();
		if (!m_pRTMP)
			return FALSE;
		RTMP_Init(m_pRTMP);
		if (!RTMP_SetupURL(m_pRTMP, szURL.STR()))
			return FALSE;
		RTMP_LogSetLevel(RTMP_LOGINFO);
		RTMP_LogSetOutput(stderr);
		RTMP_EnableWrite(m_pRTMP);
		RTMP_SetBufferMS(m_pRTMP, 60 * 1000);
		if (!RTMP_Connect(m_pRTMP, NULL))
			return FALSE;
		if (!RTMP_ConnectStream(m_pRTMP, 0))
			return FALSE;*/
		m_writer.Create("D:\\test1.flv");
		return TRUE;
	}

	BOOL MRTMPClient::Disconnect()
	{
		/*if (!m_pRTMP)
			return FALSE;
		RTMP_Close(m_pRTMP);*/
		m_writer.Close();
		return TRUE;
	}

	BOOL MRTMPClient::SendMetadata(INT cx, INT cy, INT framerate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate)
	{
		/*if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		RTMPPacket packet;
		RTMPPacket_Alloc(&packet, 1024);
		RTMPPacket_Reset(&packet);
		CHAR* body = packet.m_body;
		CHAR* ebody = body + 1024;
		body = AMF_EncodeString(body, ebody, &av_setDataFrame);
		body = AMF_EncodeString(body, ebody, &av_onMetaData);
		*body++ = AMF_OBJECT;
		body = AMF_EncodeNamedNumber(body, ebody, &av_duration, 0.0);
		body = AMF_EncodeNamedNumber(body, ebody, &av_fileSize, 0.0);
		body = AMF_EncodeNamedNumber(body, ebody, &av_width, static_cast<double>(cx));
		body = AMF_EncodeNamedNumber(body, ebody, &av_height, static_cast<double>(cy));
		body = AMF_EncodeNamedString(body, ebody, &av_videocodecid, &av_avc1);
		body = AMF_EncodeNamedNumber(body, ebody, &av_videodatarate, static_cast<double>(videoRate));
		body = AMF_EncodeNamedNumber(body, ebody, &av_framerate, static_cast<double>(framerate));
		body = AMF_EncodeNamedString(body, ebody, &av_audiocodecid, &av_mp4a);
		body = AMF_EncodeNamedNumber(body, ebody, &av_audiosamplerate, static_cast<double>(audioRate));
		body = AMF_EncodeNamedNumber(body, ebody, &av_audiosamplesize, static_cast<double>(wfx.nSamplesPerSec));
		body = AMF_EncodeNamedNumber(body, ebody, &av_audiochannels, static_cast<double>(wfx.nChannels));
		body = AMF_EncodeNamedBoolean(body, ebody, &av_stereo, wfx.nChannels == 2);
		*body++ = 0;
		*body++ = 0;
		*body++ = AMF_OBJECT_END;
		packet.m_nBodySize = body - packet.m_body;
		packet.m_packetType = RTMP_PACKET_TYPE_INFO;
		packet.m_nChannel = STREAM_CHANNEL_METADATA;
		packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
		packet.m_nInfoField2 = m_pRTMP->m_stream_id;
		BOOL bRes = FALSE;
		if (RTMP_IsConnected(m_pRTMP))
		{
			bRes = RTMP_SendPacket(m_pRTMP, &packet, FALSE) == 0;
		}
		RTMPPacket_Free(&packet);*/

		FLV_SCRIPTDATA script;
		script.duration = 0.0;
		script.filesize = 0.0;
		script.width = static_cast<double>(cx);
		script.height = static_cast<double>(cy);
		script.videodatarate = static_cast<double>(videoRate);
		script.framerate = static_cast<double>(framerate);
		script.audiosamplerate = static_cast<double>(audioRate);
		script.audiosamplerate = static_cast<double>(wfx.nSamplesPerSec);
		script.audiochannels = static_cast<double>(wfx.nChannels);
		m_writer.WriteScriptTag(script);

		return TRUE;
	}
	BOOL MRTMPClient::SendSPP(const vector<BYTE>& pps, const vector<BYTE>& sps)
	{
		//if (!m_pRTMP)
		//	return FALSE;
		//if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
		//	return FALSE;
		//RTMPPacket packet;
		//RTMPPacket_Alloc(&packet, 1024);
		//RTMPPacket_Reset(&packet);
		//memset(packet.m_body, 0, 1024);
		//BYTE* body = (BYTE*)packet.m_body;
		//INT  i = 0;
		//body[i++] = 0x17;
		//body[i++] = 0x00;
		//body[i++] = 0x00;
		//body[i++] = 0x00;
		//body[i++] = 0x00;
		////AVCDecoderConfigurationRecord
		//body[i++] = 0x01;
		//body[i++] = sps[1];
		//body[i++] = sps[2];
		//body[i++] = sps[3];
		//body[i++] = 0xFF;
		////sps
		//body[i++] = 0xE1;
		//body[i++] = (sps.size() >> 8) & 0xFF;
		//body[i++] = sps.size() & 0xFF;
		//memcpy(&body[i], &sps[0], sps.size());
		//i += sps.size();
		////pps
		//body[i++] = 0x01;
		//body[i++] = (pps.size() >> 8) & 0xFF;
		//body[i++] = (pps.size()) & 0xFF;
		//memcpy(&body[i], &pps[0], pps.size());
		//i += pps.size();
		//packet.m_nBodySize = i;
		//packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
		//packet.m_nChannel = STREAM_CHANNEL_VIDEO;
		//packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
		//packet.m_nInfoField2 = m_pRTMP->m_stream_id;
		//BOOL bRes = FALSE;
		//if (RTMP_IsConnected(m_pRTMP))
		//{
		//	bRes = RTMP_SendPacket(m_pRTMP, &packet, FALSE) == 0;
		//}
		//RTMPPacket_Free(&packet);

		vector<BYTE> data;
		data.resize(sps.size() + pps.size());
		memcpy(&data[0], &sps[0], sps.size());
		memcpy(&data[sps.size()], &pps[0], pps.size());
		m_writer.WriteH264AVC(&data[0], data.size());

		return TRUE;
	}
	BOOL MRTMPClient::SendAAC(BYTE* bits, LONG size)
	{
		//if (!m_pRTMP)
		//	return FALSE;
		//if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
		//	return FALSE;
		//RTMPPacket packet;
		//RTMPPacket_Alloc(&packet, size + 2);
		//RTMPPacket_Reset(&packet);
		//memset(packet.m_body, 0, size + 2);
		//packet.m_nBodySize = size + 2;
		//BYTE* body = (BYTE*)packet.m_body;
		//body[0] = 0xAF;
		//body[1] = 0x00;//AAC信息
		//memcpy(&body[2], bits, size);
		//packet.m_packetType = RTMP_PACKET_TYPE_AUDIO;
		//packet.m_nInfoField2 = m_pRTMP->m_stream_id;
		//packet.m_nChannel = STREAM_CHANNEL_AUDIO;
		//packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
		//BOOL bRes = FALSE;
		//if (RTMP_IsConnected(m_pRTMP))
		//{
		//	bRes = RTMP_SendPacket(m_pRTMP, &packet, FALSE) == 0;
		//}
		//RTMPPacket_Free(&packet);

		m_writer.WriteAACASC(bits, size);

		return TRUE;

	}
	BOOL MRTMPClient::SendAudio(BYTE* bits, LONG size, DWORD timestamp)
	{
		//if (!m_pRTMP)
		//	return FALSE;
		//if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
		//	return FALSE;
		////去掉ADTS头
		//bits += 7;
		//size -= 7;
		//RTMPPacket packet;
		//RTMPPacket_Alloc(&packet, size + 2);
		//RTMPPacket_Reset(&packet);
		//memset(packet.m_body, 0, size + 2);
		//packet.m_nBodySize = size + 2;
		//BYTE* body = (BYTE*)packet.m_body;
		//body[0] = 0xAF;
		//body[1] = 0x01;//原始数据
		//memcpy(&body[2], bits, size);
		//packet.m_packetType = RTMP_PACKET_TYPE_AUDIO;
		//packet.m_nInfoField2 = m_pRTMP->m_stream_id;
		//packet.m_nChannel = STREAM_CHANNEL_AUDIO;
		//packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		//packet.m_nTimeStamp = timestamp;
		//BOOL bRes = FALSE;
		//if (RTMP_IsConnected(m_pRTMP))
		//{
		//	bRes = RTMP_SendPacket(m_pRTMP, &packet, FALSE) == 0;
		//}
		//RTMPPacket_Free(&packet);

		m_writer.WriteAACRaw(bits, size, timestamp);

		return TRUE;
	}
	BOOL MRTMPClient::SendVideo(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp)
	{
		//if (!m_pRTMP)
		//	return FALSE;
		//if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
		//	return FALSE;
		//switch (bits[2])
		//{
		//case 0x00:
		//{
		//	bits += 4;
		//	size -= 4;
		//}
		//break;
		//case 0x01:
		//{
		//	bits += 3;
		//	size -= 3;
		//}
		//break;
		//}
		//RTMPPacket packet;
		//RTMPPacket_Alloc(&packet, size + 9);
		//RTMPPacket_Reset(&packet);
		//memset(packet.m_body, 0, size + 9);
		//packet.m_nBodySize = size + 9;
		//CHAR* body = (CHAR*)packet.m_body;
		//memset(body, 0, size + 9);
		//body[0] = dwFrameType;
		//body[1] = 0x01;// AVC NALU  
		//body[2] = 0x00;
		//body[3] = 0x00;
		//body[4] = 0x00;
		//body[5] = (size >> 24) & 0xFF;
		//body[6] = (size >> 16) & 0xFF;
		//body[7] = (size >> 8) & 0xFF;
		//body[8] = (size) & 0xFF;
		//memcpy(&body[9], bits, size);
		//packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
		//packet.m_nInfoField2 = m_pRTMP->m_stream_id;
		//packet.m_nChannel = STREAM_CHANNEL_VIDEO;
		//packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		//packet.m_nTimeStamp = timestamp;
		//BOOL bRes = FALSE;
		//if (RTMP_IsConnected(m_pRTMP))
		//{
		//	bRes = RTMP_SendPacket(m_pRTMP, &packet, FALSE) == 0;
		//}
		//RTMPPacket_Free(&packet);
		m_writer.WriteH264NALU(dwFrameType == 0x17 ? 1 : 2, bits, size, timestamp, timestamp);

		return TRUE;
	}

	BOOL MRTMPClient::IsConnected()
	{
		if (!m_pRTMP)
			return FALSE;
		return RTMP_IsConnected(m_pRTMP);
	}

	BOOL MRTMPClient::IsTimedout()
	{
		if (!m_pRTMP)
			return FALSE;
		return !RTMP_IsTimedout(m_pRTMP);
	}

	BOOL MRTMPClient::Reconnect()
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_ReconnectStream(m_pRTMP, 0))
			return FALSE;
		return TRUE;
	}
}
