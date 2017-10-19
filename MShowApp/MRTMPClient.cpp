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
		:m_pRTMP(NULL),
		m_dwPreviousSize(0)
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
		m_writer.Close();
	}

	DWORD MRTMPClient::GetTime()
	{
		return RTMP_GetTime();
	}

	BOOL MRTMPClient::Connect(const TinyString& szURL)
	{
		if (m_pRTMP != NULL)
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
			return FALSE;

		m_writer.Create("D:\\test1.flv");

		return TRUE;
	}

	BOOL MRTMPClient::Disconnect()
	{
		if (!m_pRTMP)
			return FALSE;
		RTMP_Close(m_pRTMP);
		m_writer.Close();
		return TRUE;
	}

	BOOL MRTMPClient::SendScript(INT cx, INT cy, INT framerate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate)
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		TinyBufferArray<BYTE> buffer;
		FLV_SCRIPTDATA script;
		script.duration = 0.0;
		script.filesize = 0.0;
		script.width = static_cast<double>(cx);
		script.height = static_cast<double>(cy);
		script.videodatarate = static_cast<double>(videoRate);
		script.framerate = static_cast<double>(framerate);
		script.audiodatarate = static_cast<double>(audioRate);
		script.audiosamplerate = static_cast<double>(wfx.nSamplesPerSec);
		script.audiochannels = static_cast<double>(wfx.nChannels);
		BuildScript(script, buffer);
		RTMP_Write(m_pRTMP, reinterpret_cast<CHAR*>(buffer.GetPointer()), buffer.GetSize());

		m_writer.WriteScriptTag(script);

		return TRUE;
	}
	BOOL MRTMPClient::SendH264AVC(const vector<BYTE>& pps, const vector<BYTE>& sps)
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		TinyBufferArray<BYTE> buffer;
		BuildH264AVC(pps, sps, buffer);
		RTMP_Write(m_pRTMP, reinterpret_cast<CHAR*>(buffer.GetPointer()), buffer.GetSize());

		m_writer.WriteH264AVC(sps, pps);

		return TRUE;
	}
	BOOL MRTMPClient::SendAACASC(BYTE* bits, LONG size)
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		TinyBufferArray<BYTE> buffer;
		BuildAACASC(bits, size, buffer);
		RTMP_Write(m_pRTMP, reinterpret_cast<CHAR*>(buffer.GetPointer()), buffer.GetSize());

		m_writer.WriteAACASC(bits, size);

		return TRUE;

	}
	BOOL MRTMPClient::SendAACRaw(BYTE* bits, LONG size, DWORD timestamp)
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		TinyBufferArray<BYTE> buffer;
		BuildAACRaw(bits, size, timestamp, buffer);
		RTMP_Write(m_pRTMP, reinterpret_cast<CHAR*>(buffer.GetPointer()), buffer.GetSize());

		m_writer.WriteAACRaw(bits, size, timestamp);

		return TRUE;
	}
	BOOL MRTMPClient::SendH264NALU(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp)
	{
		if (!m_pRTMP)
			return FALSE;
		if (!RTMP_IsConnected(m_pRTMP) || RTMP_IsTimedout(m_pRTMP))
			return FALSE;
		TinyBufferArray<BYTE> buffer;
		BuildH264NALU(dwFrameType, bits, size, timestamp, buffer);
		RTMP_Write(m_pRTMP, reinterpret_cast<CHAR*>(buffer.GetPointer()), buffer.GetSize());

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
	BOOL MRTMPClient::BuildScript(const FLV_SCRIPTDATA& script, TinyBufferArray<BYTE>& buffer)
	{
		FLV_TAG_HEADER tag = { 0 };
		CHAR bits[2048];
		CHAR *pBEGIN = bits;
		CHAR *pEND = bits + sizeof(bits);
		pBEGIN = AMF_EncodeString(pBEGIN, pEND, &av_onMetaData);
		*pBEGIN++ = AMF_OBJECT;
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_duration, script.duration);
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_fileSize, script.filesize);
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_width, script.width);
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_height, script.height);
		pBEGIN = AMF_EncodeNamedString(pBEGIN, pEND, &av_videocodecid, &av_avc1);//h264
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_videodatarate, script.videodatarate);//最大比特率
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_framerate, script.framerate);
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_audiodatarate, script.audiodatarate); //类似128kb/s
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_audiosamplerate, script.audiosamplerate);
		pBEGIN = AMF_EncodeNamedNumber(pBEGIN, pEND, &av_audiosamplesize, script.audiosamplesize);
		pBEGIN = AMF_EncodeNamedBoolean(pBEGIN, pEND, &av_stereo, static_cast<INT>(script.audiochannels) == 2);
		*pBEGIN++ = 0;
		*pBEGIN++ = 0;
		*pBEGIN++ = AMF_OBJECT_END;
		DWORD dwSize = pBEGIN - bits;
		tag.size[0] = dwSize >> 16 & 0xFF;
		tag.size[1] = dwSize >> 8 & 0xFF;
		tag.size[2] = dwSize & 0xFF;
		tag.type = FLV_SCRIPT;
		buffer.Add(reinterpret_cast<BYTE*>(&tag), sizeof(tag));
		buffer.Add(reinterpret_cast<BYTE*>(bits), dwSize);
		m_dwPreviousSize = dwSize + sizeof(FLV_TAG_HEADER);
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		buffer.Add(reinterpret_cast<BYTE*>(&dwPreviousSize), sizeof(dwPreviousSize));
		return TRUE;
	}
	BOOL MRTMPClient::BuildH264AVC(const vector<BYTE>& pps, const vector<BYTE>& sps, TinyBufferArray<BYTE>& buffer)
	{
		FLV_PACKET video = { 0 };
		video.codeID = FLV_CODECID_H264;
		video.codeType = 1;
		video.packetType = 0;//AVC sequence header
		vector<BYTE> bits;
		bits.push_back(0x01);
		bits.push_back(sps[0]);
		bits.push_back(sps[1]);
		bits.push_back(sps[2]);
		bits.push_back(0xFF);
		bits.push_back(0xE1);
		INT size = sps.size();
		bits.push_back((size >> 8) & 0xFF);
		bits.push_back(size & 0xFF);
		for (INT i = 0;i < size;i++)
		{
			bits.push_back(sps[i]);
		}
		bits.push_back(1);
		size = pps.size();
		bits.push_back((size >> 8) & 0xFF);
		bits.push_back(size & 0xFF);
		for (INT i = 0;i < size;i++)
		{
			bits.push_back(pps[i]);
		}
		return BuildVideoTag(video, &bits[0], bits.size(), buffer);
	}
	BOOL MRTMPClient::BuildH264NALU(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp, TinyBufferArray<BYTE>& buffer)
	{
		FLV_PACKET video = { 0 };
		video.codeID = FLV_CODECID_H264;
		video.codeType = dwFrameType == 0x17 ? 1 : 2;
		video.packetType = 1;//AVC NALU
		video.dts = timestamp;
		video.pts = timestamp;
		vector<BYTE> data;
		data.push_back((size >> 24) & 0xFF);
		data.push_back((size >> 16) & 0xFF);
		data.push_back((size >> 8) & 0xFF);
		data.push_back(size & 0xFF);
		data.insert(data.end(), bits, bits + size);
		return BuildVideoTag(video, &data[0], data.size(), buffer);
	}
	BOOL MRTMPClient::BuildAACASC(BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer)
	{
		FLV_PACKET audio = { 0 };
		audio.bitsPerSample = 3;
		audio.samplesPerSec = 1;
		audio.channel = 1;
		audio.codeID = FLV_CODECID_AAC;
		audio.packetType = 0;//AAC Sequence header
		return BuildAudioTag(audio, bits, size, buffer);
	}
	BOOL MRTMPClient::BuildAACRaw(BYTE* bits, LONG size, DWORD timestamp, TinyBufferArray<BYTE>& buffer)
	{
		FLV_PACKET audio = { 0 };
		audio.bitsPerSample = 3;
		audio.samplesPerSec = 1;
		audio.channel = 1;
		audio.codeID = FLV_CODECID_AAC;
		audio.packetType = 1;//AAC Raw
		audio.dts = timestamp;
		audio.pts = timestamp;
		//去掉ADTS
		bits += 7;
		size -= 7;
		return BuildAudioTag(audio, bits, size, buffer);
	}

	BOOL MRTMPClient::BuildAudioTag(FLV_PACKET& packet, BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer)
	{
		FLV_TAG_HEADER tag = { 0 };
		tag.type = FLV_AUDIO;
		DWORD dwSize = size + 2;
		tag.size[0] = dwSize >> 16 & 0xFF;
		tag.size[1] = dwSize >> 8 & 0xFF;
		tag.size[2] = dwSize & 0xFF;
		tag.timestamp[0] = packet.dts >> 16 & 0xFF;
		tag.timestamp[1] = packet.dts >> 8 & 0xFF;
		tag.timestamp[2] = packet.dts & 0xFF;
		tag.timestampex = packet.dts >> 24 & 0xFF;
		buffer.Add(reinterpret_cast<BYTE*>(&tag), sizeof(tag));
		FLV_TAG_AUDIO audio = { 0 };
		audio.bitsPerSample = packet.bitsPerSample;
		audio.channel = packet.channel;
		audio.codeID = packet.codeID;
		audio.samplesPerSec = packet.samplesPerSec;
		buffer.Add(reinterpret_cast<BYTE*>(&audio), sizeof(audio));
		buffer.Add(&packet.packetType, sizeof(packet.packetType));
		buffer.Add(bits, size);
		m_dwPreviousSize = dwSize + sizeof(FLV_TAG_HEADER);
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		buffer.Add(reinterpret_cast<BYTE*>(&dwPreviousSize), sizeof(dwPreviousSize));
		return TRUE;
	}
	BOOL MRTMPClient::BuildVideoTag(FLV_PACKET& packet, BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer)
	{
		FLV_TAG_HEADER tag = { 0 };
		tag.type = FLV_VIDEO;
		DWORD dwSize = size + 5;
		tag.size[0] = dwSize >> 16 & 0xFF;
		tag.size[1] = dwSize >> 8 & 0xFF;
		tag.size[2] = dwSize & 0xFF;
		tag.timestamp[0] = packet.dts >> 16 & 0xFF;
		tag.timestamp[1] = packet.dts >> 8 & 0xFF;
		tag.timestamp[2] = packet.dts & 0xFF;
		tag.timestampex = packet.dts >> 24 & 0xFF;
		buffer.Add(reinterpret_cast<BYTE*>(&tag), sizeof(tag));
		FLV_TAG_VIDEO video = { 0 };
		video.codeID = packet.codeID;
		video.codeType = packet.codeType;
		buffer.Add(reinterpret_cast<BYTE*>(&video), sizeof(video));
		buffer.Add(&packet.packetType, sizeof(packet.packetType));
		BYTE cts[3];
		cts[0] = (packet.pts - packet.dts) >> 16 & 0xFF;
		cts[1] = (packet.pts - packet.dts) >> 8 & 0xFF;
		cts[2] = (packet.pts - packet.dts) & 0xFF;
		buffer.Add(cts, 3);
		buffer.Add(bits, size);
		m_dwPreviousSize = dwSize + sizeof(FLV_TAG_HEADER);
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		buffer.Add(reinterpret_cast<BYTE*>(&dwPreviousSize), sizeof(dwPreviousSize));
		return TRUE;
	}
}
