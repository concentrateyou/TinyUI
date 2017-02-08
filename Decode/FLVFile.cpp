#include "stdafx.h"
#include "FLVFile.h"

namespace Decode
{
	const INT SampleRates[16] =
	{
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000, 7350
	};
	//////////////////////////////////////////////////////////////////////////
	FLVFile::FLVFile(LPCSTR pzFile)
		:m_hFile(NULL)
	{
		fopen_s(&m_hFile, pzFile, "rb");
	}
	FLVFile::~FLVFile()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
	}
	void FLVFile::ParseVideo(BYTE* data, INT size)
	{
		FLV_TAG_VIDEO* video = reinterpret_cast<FLV_TAG_VIDEO*>(data);

	}
	void FLVFile::ParseAudio(BYTE* data, INT size)
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
	void FLVFile::ParseScript(BYTE* data, INT size)
	{

	}

	void FLVFile::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		if (m_aac == NULL)
		{
			m_aac.Reset(new AACDecode());
			DWORD dwSamplesPerSec = 44100;
			switch (audio->samplesPerSec)
			{
			case 0:
				dwSamplesPerSec = 5500;
				break;
			case 1:
				dwSamplesPerSec = 11000;
				break;
			case 2:
				dwSamplesPerSec = 22000;
				break;
			case 3:
				dwSamplesPerSec = 44100;
				break;
			}
			DWORD dwBitsPerSample = 16;
			switch (audio->bitsPerSample)
			{
			case 0:
				dwBitsPerSample = 8;
				break;
			case 1:
				dwBitsPerSample = 16;
				break;
			}
			m_aac->Open(dwBitsPerSample, dwSamplesPerSec);
		}
		if (aacPacketType == 0)
		{
			BYTE audioObjectType = ((*bits & 0xF8) >> 3) - 1;
			BYTE samplingFrequencyIndex = ((*bits & 0x07) << 1) | (*(bits + 1) >> 7);
			BYTE channelConfiguration = (*(bits + 1) >> 3) & 0x0F;
		}
		if (aacPacketType == 1)
		{
			DWORD dwINC = 0;
			m_aac->Decode(bits, size, dwINC);
		}

	}
	void FLVFile::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		EVENT_AUDIO(data, size, audio);
	}
	void FLVFile::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		EVENT_AUDIO(data, size, audio);
	}

	BOOL FLVFile::Decode()
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

