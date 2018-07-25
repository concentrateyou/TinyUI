#include "stdafx.h"
#include "FLVParser.h"
#include "RTMPStream.h"

namespace Decode
{
	FLVParser::FLVParser()
		:m_hFile(NULL),
		m_minusOne(4),
		m_bAudio(FALSE),
		m_bVideo(FALSE),
		m_timestamp(0),
		m_index(0),
		m_h264File(NULL)
	{

	}
	FLVParser::~FLVParser()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}

		if (m_h264File != NULL)
		{
			fclose(m_h264File);
			m_h264File = NULL;
		}

	}
	BOOL FLVParser::Open(LPCSTR pzFile)
	{
		Close();
		fopen_s(&m_hFile, pzFile, "rb");
		fopen_s(&m_h264File, "D:\\test1.h264", "wb+");
		return m_hFile != NULL;
	}
	BOOL FLVParser::Close()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
		if (m_h264File != NULL)
		{
			fflush(m_h264File);
			fclose(m_h264File);
			m_h264File = NULL;
		}
		return TRUE;
	}
	BOOL FLVParser::ParseVideo(BYTE* data, INT size)
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
	BOOL FLVParser::ParseAudio(BYTE* data, INT size)
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
	BOOL FLVParser::ParseScript(BYTE* data, INT size)
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
	BOOL FLVParser::ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size)
	{
		return TRUE;
	}
	BOOL FLVParser::ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size)
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
			fwrite(buffer.GetPointer(), sizeof(BYTE), buffer.GetSize(), m_h264File);
		}
		if (aacPacketType == 1)
		{
			return ParseNALU(video, &cts, bits, size);
		}
		return TRUE;
	}
	BOOL FLVParser::ParseNALU(FLV_TAG_VIDEO* video, INT* cts, BYTE* data, INT size)
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
				fwrite((void*)val, sizeof(BYTE), sizeofNALU + 4, m_h264File);
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

	INT64 g_time = -1;

	BOOL FLVParser::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
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

			TRACE("AAC DTS:%lld, DIFF:%lld\n", m_timestamp, m_timestamp - g_time);

			g_time = m_timestamp;
		}
		return TRUE;
	}
	BOOL FLVParser::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		return TRUE;
	}
	BOOL FLVParser::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		return TRUE;
	};

	BOOL FLVParser::Parse()
	{
		FLV_HEADER header = { 0 };
		ASSERT(sizeof(FLV_HEADER) != fread(&header, sizeof(FLV_HEADER), 1, m_hFile));
		if (strncmp("FLV", (CHAR*)header.signature, 3) != 0)
			return FALSE;
		m_bAudio = (header.streamType & 0x04) != 0;
		m_bVideo = (header.streamType & 0x01) != 0;
		INT offset = ToINT32(header.offset);
		fseek(m_hFile, offset, SEEK_SET);
		for (;;)
		{
			UINT tagSize = 0;
			if (fread(&tagSize, sizeof(UINT), 1, m_hFile) <= 0)
				break;
			tagSize = htonl(tagSize);
			FLV_TAG_HEADER tag = { 0 };
			if (fread(&tag, sizeof(FLV_TAG_HEADER), 1, m_hFile) <= 0)
				break;
			INT size = ToINT24(tag.size);
			if (size > 0)
			{
				TinyScopedArray<BYTE> data(new BYTE[size]);
				if (fread(data, size, 1, m_hFile) <= 0)
					break;
				switch (tag.type)
				{
				case FLV_AUDIO:
				{
					m_timestamp = static_cast<LONGLONG>(static_cast<UINT32>(ToINT24(tag.timestamp) | (tag.timestampex << 24)));
					if (!ParseAudio(data, size))
						return FALSE;
				}
				break;
				case FLV_VIDEO:
				{
					m_timestamp = static_cast<LONGLONG>(static_cast<UINT32>(ToINT24(tag.timestamp) | (tag.timestampex << 24)));
					if (!ParseVideo(data, size))
						return FALSE;
				}
				break;
				case FLV_SCRIPT:
				{
					ParseScript(data, size);
				}
				break;
				default:
					break;
				}
			}
		}
		return TRUE;
	}
}

