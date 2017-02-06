#include "stdafx.h"
#include "FLVDecode.h"

namespace Decode
{
	FLVDecode::FLVDecode(LPCSTR pzFile)
		:m_hFile(NULL)
	{
		fopen_s(&m_hFile, pzFile, "rb");
	}
	FLVDecode::~FLVDecode()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
	}
	void FLVDecode::ParseVideo(BYTE* data, INT size)
	{
		FLV_TAG_VIDEO* video = reinterpret_cast<FLV_TAG_VIDEO*>(data);

	}
	void FLVDecode::ParseAudio(BYTE* data, INT size)
	{
		//目前只支持MP3,AAC和PCM
		FLV_TAG_AUDIO* audio = reinterpret_cast<FLV_TAG_AUDIO*>(data);
		switch (audio->codeID)
		{
		case FLV_CODECID_PCM:
			ParsePCM(audio, data + 1, size - 1);
			break;
		case FLV_CODECID_AAC:
			ParseAAC(audio, data + 1, size - 1);
			break;
		case FLV_CODECID_MP3:
			ParseMP3(audio, data + 1, size - 1);
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
		default:
			break;
		}
	}
	void FLVDecode::ParseScript(BYTE* data, INT size)
	{
		//TODO
	}

	void FLVDecode::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{

	}
	void FLVDecode::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{

	}
	void FLVDecode::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{

	}

	BOOL FLVDecode::Decode()
	{
		ASSERT(sizeof(FLV_HEADER) != fread(&m_header, sizeof(FLV_HEADER), 1, m_hFile));
		if (strncmp("FLV", (CHAR*)m_header.signature, 3) != 0)
			return FALSE;
		m_bAudio = (m_header.streamType & 0x04) != 0;
		m_bVideo = (m_header.streamType & 0x01) != 0;
		INT offset = Utility::ToINT32(m_header.offset);
		fseek(m_hFile, offset, SEEK_SET);
		for (;;)
		{
			UINT tagSize = 0;
			ASSERT(sizeof(UINT) != fread(&tagSize, sizeof(UINT), 1, m_hFile));
			tagSize = htonl(tagSize);
			FLV_TAG_HEADER tag = { 0 };
			ASSERT(sizeof(FLV_TAG_HEADER) != fread(&tag, sizeof(FLV_TAG_HEADER), 1, m_hFile));
			INT size = Utility::ToINT24(tag.size);
			if (size > 0)
			{
				TinyScopedArray<BYTE> data(new BYTE[size]);
				ASSERT(size != fread(data, size, 1, m_hFile));
				switch (tag.type)
				{
				case 0x08:
					ParseAudio(data, size);
					break;
				case 0x09:
					ParseVideo(data, size);
					break;
				case 0x12:
					ParseScript(data, size);
					break;
				default:
					break;
				}
			}
		}
		return TRUE;
	}
}

