#include "stdafx.h"
#include "RTMPReader.h"

namespace Decode
{
	RTMPReader::RTMPReader()
		:m_minusOne(4),
		m_timestamp(0)
	{

	}
	RTMPReader::~RTMPReader()
	{
		Close(INFINITE);
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
		m_close.SetEvent();
		RTMP_Close(&m_sRTMP);
		return TinyTaskBase::Close(dwMs);
	}
	BOOL RTMPReader::ParseVideo(BYTE* data, INT size)
	{
		BOOL bRes = FALSE;
		FLV_TAG_VIDEO* video = reinterpret_cast<FLV_TAG_VIDEO*>(data);
		switch (video->codeID)
		{
		case FLV_CODECID_H264:
			bRes = ParseH264(video, data + 1, size - 1);
			break;
		case FLV_CODECID_MPEG4:
			bRes = ParseMPEG4(video, data + 1, size - 1);
			break;
		case FLV_CODECID_H263:
		case FLV_CODECID_SCREEN:
		case FLV_CODECID_VP6:
		case FLV_CODECID_VP6A:
		case FLV_CODECID_SCREEN2:
		case FLV_CODECID_REALH263:
			break;
		}
		return TRUE;
	}
	BOOL RTMPReader::ParseAudio(BYTE* data, INT size)
	{
		BOOL bRes = FALSE;
		//目前只支持MP3,AAC和PCM
		FLV_TAG_AUDIO* audio = reinterpret_cast<FLV_TAG_AUDIO*>(data);
		switch (audio->codeID)
		{
		case FLV_CODECID_PCM:
			bRes = ParsePCM(audio, data + 1, size - 1);
			break;
		case FLV_CODECID_AAC:
			bRes = ParseAAC(audio, data + 1, size - 1);
			break;
		case FLV_CODECID_MP3:
			bRes = ParseMP3(audio, data + 1, size - 1);
			break;
		case FLV_CODECID_SPEEX:
		case FLV_CODECID_ADPCM:
		case FLV_CODECID_PCM_LE:
		case FLV_CODECID_NELLYMOSER_16KHZ_MONO:
		case FLV_CODECID_NELLYMOSER_8KHZ_MONO:
		case FLV_CODECID_NELLYMOSER:
		case FLV_CODECID_PCM_ALAW:
		case FLV_CODECID_PCM_MULAW:
			break;
		}
		return bRes;
	}
	BOOL RTMPReader::ParseScript(BYTE* data, INT size)
	{
		AMFObject metaObj;
		if (AMF_Decode(&metaObj, reinterpret_cast<CHAR*>(data), size, FALSE) > 0)
		{
			FLV_SCRIPTDATA	script = { 0 };
			for (INT i = 0; i < metaObj.o_num; i++)
			{
				string val;
				AMFObjectProperty* prop = &metaObj.o_props[i];
				if (prop->p_type == AMF_OBJECT || prop->p_type == AMF_ECMA_ARRAY)
				{
					for (INT j = 0; j < prop->p_vu.p_object.o_num; j++)
					{
						AMFObjectProperty * objProp = &prop->p_vu.p_object.o_props[j];
						if (objProp->p_name.av_len > 0)
						{
							val.resize(objProp->p_name.av_len);
							val = objProp->p_name.av_val;
							if ("width" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.width = objProp->p_vu.p_number;
							}
							if ("height" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.height = objProp->p_vu.p_number;
							}
							if ("duration" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.duration = objProp->p_vu.p_number * 1000000;
							}
							if ("framerate" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.framerate = objProp->p_vu.p_number;
							}
							if ("filesize" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.filesize = objProp->p_vu.p_number;
							}
							if ("datasize" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.datasize = objProp->p_vu.p_number;
							}
							if ("audiosize" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiosize = objProp->p_vu.p_number;
							}
							if ("videosize" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.videosize = objProp->p_vu.p_number;
							}
							if ("videocodecid" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.videocodecid = objProp->p_vu.p_number;
							}
							if ("videodatarate" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.videodatarate = objProp->p_vu.p_number;
							}
							if ("audiocodecid" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiocodecid = objProp->p_vu.p_number;
							}
							if ("audiodatarate" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiodatarate = objProp->p_vu.p_number;
							}
							if ("audiosamplesize" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiosamplesize = objProp->p_vu.p_number;
							}
							if ("audiosamplerate" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiosamplerate = objProp->p_vu.p_number;
							}
						}
					}
				}
			}
			AMF_Reset(&metaObj);
			EVENT_SCRIPT(&script);
		}
		return TRUE;
	}
	BOOL RTMPReader::ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size)
	{
		return TRUE;
	}
	BOOL RTMPReader::ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size)
	{
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		size -= 1;
		INT cts = (ToINT24(bits) + 0xFF800000) ^ 0xFF800000;
		bits += 3;
		size -= 3;
		if (aacPacketType == 0)
		{
			TinyBufferArray<BYTE> buffer;
			AVCDecoderConfigurationRecord avcconfig;
			avcconfig.ConfigurationVersion = *bits++;
			avcconfig.AVCProfileIndication = *bits++;
			avcconfig.ProfileCompatibility = *bits++;
			avcconfig.AVCLevelIndication = *bits++;
			avcconfig.LengthSizeMinusOne = *bits++ & 0x03 + 1;//一般是4
			avcconfig.NumOfSequenceParameterSets = *bits++ & 0x1F;//一般是1
			m_minusOne = avcconfig.LengthSizeMinusOne;
			for (INT i = 0; i < avcconfig.NumOfSequenceParameterSets; i++)
			{
				BYTE* val = reinterpret_cast<BYTE*>(const_cast<UINT32*>(&H264StartCode));
				buffer.Add(val, 4);
				USHORT s = *bits++ << 8;
				s |= *bits++;
				buffer.Add(bits, s);
				bits += s;
			}
			avcconfig.NumOfPictureParameterSets = *bits++;
			for (INT i = 0; i < avcconfig.NumOfPictureParameterSets; i++)
			{
				buffer.Add(reinterpret_cast<BYTE*>(const_cast<UINT32*>(&H264StartCode)), 4);
				USHORT s = *bits++ << 8;
				s |= *bits++;
				buffer.Add(bits, s);
				bits += s;
			}
			FLV_PACKET packet;
			packet.dts = m_timestamp;
			packet.pts = m_timestamp + cts;
			packet.codeID = video->codeID;
			packet.codeType = video->codeType;
			packet.packetType = FLV_AVCDecoderConfigurationRecord;
			EVENT_VIDEO(buffer.GetPointer(), buffer.GetSize(), &packet);
		}
		if (aacPacketType == 1)
		{
			return ParseNALU(video, &cts, bits, size);
		}
		return TRUE;
	}
	BOOL RTMPReader::ParseNALU(FLV_TAG_VIDEO* video, INT* cts, BYTE* data, INT size)
	{
		BYTE* bits = data;
		INT offset = 0;
		INT sizeofNALU = 0;
		FLV_PACKET packet = { 0 };
		for (;;)
		{
			if (offset >= size)
				break;
			switch (m_minusOne)
			{
			case 4:
			{
				sizeofNALU = ToINT32(bits);
				bits += 4;
				offset += 4;
				TinyScopedPtr<BYTE> val(new BYTE[sizeofNALU + 4]);
				memcpy(val, &H264StartCode, 4);
				memcpy(val + 4, bits, sizeofNALU);
				packet.codeID = video->codeID;
				packet.codeType = video->codeType;
				packet.packetType = FLV_NALU;
				packet.dts = m_timestamp;
				packet.pts = m_timestamp + *cts;
				EVENT_VIDEO(val, sizeofNALU + 4, &packet);
				bits += sizeofNALU;
				offset += sizeofNALU;
			}
			break;
			case 3:
			{
				sizeofNALU = ToINT24(bits);
				bits += 3;
				offset += 3;
				TinyScopedPtr<BYTE> val(new BYTE[sizeofNALU + 4]);
				memcpy(val, &H264StartCode, 4);
				memcpy(val + 4, bits, sizeofNALU);
				packet.codeID = video->codeID;
				packet.codeType = video->codeType;
				packet.packetType = FLV_NALU;
				packet.dts = m_timestamp;
				packet.pts = m_timestamp + *cts;
				EVENT_VIDEO(val, sizeofNALU + 4, &packet);
				bits += sizeofNALU;
				offset += sizeofNALU;
			}
			break;
			case 2:
			{
				sizeofNALU = ToINT16(bits);
				bits += 2;
				offset += 2;
				TinyScopedPtr<BYTE> val(new BYTE[sizeofNALU + 4]);
				memcpy(val, &H264StartCode, 4);
				memcpy(val + 4, bits, sizeofNALU);
				packet.codeID = video->codeID;
				packet.codeType = video->codeType;
				packet.packetType = FLV_NALU;
				packet.dts = m_timestamp;
				packet.pts = m_timestamp + *cts;
				EVENT_VIDEO(val, sizeofNALU + 4, &packet);
				bits += sizeofNALU;
				offset += sizeofNALU;
			}
			break;
			case 1:
			{
				sizeofNALU = ToINT8(bits);
				bits += 1;
				offset += 1;
				TinyScopedPtr<BYTE> val(new BYTE[sizeofNALU + 4]);
				memcpy(val, &H264StartCode, 4);
				memcpy(val + 4, bits, sizeofNALU);
				packet.codeID = video->codeID;
				packet.codeType = video->codeType;
				packet.packetType = FLV_NALU;
				packet.dts = m_timestamp;
				packet.pts = m_timestamp + *cts;
				EVENT_VIDEO(val, sizeofNALU + 4, &packet);
				bits += sizeofNALU;
				offset += sizeofNALU;
			}
			break;
			}
		}
		return TRUE;
	}
	BOOL RTMPReader::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		ASSERT(size >= 2);
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		size -= 1;
		FLV_PACKET packet = { 0 };
		if (aacPacketType == 0)
		{
			packet.dts = m_timestamp;
			packet.pts = m_timestamp;
			packet.bitsPerSample = audio->bitsPerSample;
			packet.channel = audio->channel;
			packet.codeID = FLV_CODECID_AAC;
			packet.packetType = FLV_AudioSpecificConfig;
			EVENT_AUDIO(bits, size, &packet);
		}
		if (aacPacketType == 1)
		{
			packet.dts = m_timestamp;
			packet.pts = m_timestamp;
			packet.bitsPerSample = audio->bitsPerSample;
			packet.channel = audio->channel;
			packet.codeID = FLV_CODECID_AAC;
			packet.packetType = FLV_AACRaw;
			EVENT_AUDIO(bits, size, &packet);
		}
		return TRUE;
	}
	BOOL RTMPReader::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		return TRUE;
	}
	BOOL RTMPReader::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		return TRUE;
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
			if (packet.m_packetType == FLV_AUDIO)
			{
				m_timestamp = packet.m_nTimeStamp;
				if (!ParseAudio((BYTE*)packet.m_body, packet.m_nBodySize))
					break;
			}
			if (packet.m_packetType == FLV_VIDEO)
			{
				m_timestamp = packet.m_nTimeStamp;
				if (!ParseVideo((BYTE*)packet.m_body, packet.m_nBodySize))
					break;
			}
			if (packet.m_packetType == FLV_AUDIO)
			{
				ParseScript((BYTE*)packet.m_body, packet.m_nBodySize);
			}
		}
	}
}