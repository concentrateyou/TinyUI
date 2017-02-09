#include "stdafx.h"
#include "FLVFile.h"

namespace Decode
{
	FLVFile::FLVFile()
		:m_hFile(NULL)
	{
		m_audioDone.Reset(new Delegate<void(BYTE*, LONG, LPVOID)>(this, &FLVFile::OnAudioDone));
	}
	FLVFile::~FLVFile()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
	}
	BOOL FLVFile::Open(LPCSTR pzFile)
	{
		Close();
		fopen_s(&m_hFile, pzFile, "rb");
		return TRUE;
	}
	BOOL FLVFile::Close()
	{
		if (m_hFile != NULL)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
		if (m_aac != NULL)
		{
			m_aac->EVENT_DONE -= m_audioDone;
			m_audioDone.Reset(NULL);
		}
		return TRUE;
	}
	BOOL FLVFile::ParseVideo(BYTE* data, INT size)
	{
		BOOL bRes = FALSE;
		FLV_TAG_VIDEO* video = reinterpret_cast<FLV_TAG_VIDEO*>(data);
		switch (video->codeID)
		{
		case FLV_CODECID_H264:
			bRes = ParseH264(video, data + 1, size - 1);
			break;
		case FLV_CODECID_H263:
		case FLV_CODECID_SCREEN:
		case FLV_CODECID_VP6:
		case FLV_CODECID_VP6A:
		case FLV_CODECID_SCREEN2:
		case FLV_CODECID_REALH263:
		case FLV_CODECID_MPEG4:
			break;
		}
		return TRUE;
	}
	BOOL FLVFile::ParseAudio(BYTE* data, INT size)
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
	BOOL FLVFile::ParseScript(BYTE* data, INT size)
	{

		return TRUE;
	}
	BOOL FLVFile::ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size)
	{
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		INT dwCompositionTime = 0;
		memcpy(reinterpret_cast<BYTE*>(&dwCompositionTime), bits, 3);
		bits += 3;
		if (m_h264 == NULL)
		{
			m_h264.Reset(new H264Decode());
			m_h264->EVENT_DONE += m_videoDone;
		}
		if (aacPacketType == 0)
		{
			m_avcConfig.ConfigurationVersion = *bits++;
			m_avcConfig.AVCProfileIndication = *bits++;
			m_avcConfig.ProfileCompatibility = *bits++;
			m_avcConfig.AVCLevelIndication = *bits++;
			m_avcConfig.LengthSizeMinusOne = *bits++ & 0x03 + 1;//一般是4
			BYTE numOfSequenceParameterSets = *bits++ & 0x1F;//一般是1
			m_sps.resize(numOfSequenceParameterSets);
			for (INT i = 0; i < numOfSequenceParameterSets; i++)
			{
				USHORT s = *bits++ << 8;
				s |= *bits++;
				m_sps[i].resize(s);
				memcpy(&m_sps[i][i], bits, s);
				bits += s;
			}
			BYTE numOfPictureParameterSets = *bits++;
			m_pps.resize(numOfPictureParameterSets);
			for (INT i = 0; i < numOfPictureParameterSets; i++)
			{
				USHORT s = *bits++ << 8;
				s |= *bits++;
				m_pps[i].resize(s);
				memcpy(&m_pps[i][i], bits, s);
				bits += s;
			}
		}
		if (aacPacketType == 1)
		{
			return ParseNALU(video, data, size);
		}
		if (aacPacketType == 2)
		{
			//TODO
		}
		return TRUE;
	}
	BOOL FLVFile::ParseNALU(FLV_TAG_VIDEO* video, BYTE* data, INT size)
	{
		for (;;)
		{
			switch (m_avcConfig.LengthSizeMinusOne)
			{
			case 4:
			{

			}
			break;
			case 3:
			{

			}
			break;
			case 2:
			{

			}
			break;
			default:
			{

			}
			break;
			}
		}
		return TRUE;
	}
	BOOL FLVFile::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		ASSERT(size >= 2);
		BYTE* bits = data;
		size -= 1;
		BYTE aacPacketType = *bits++;
		if (m_aac == NULL)
		{
			m_aac.Reset(new AACDecode());
			m_aac->EVENT_DONE += m_audioDone;
		}
		if (aacPacketType == 0)
		{
			ULONG sampleRate = 0;
			BYTE channel = 0;
			if (m_aac->Open(bits, size - 1, audio->bitsPerSample == 0 ? 8 : 16))
			{
				WAVEFORMATEX sFMT = m_aac->GetFormat();
				EVENT_AUDIO(NULL, 0, &sFMT);
				return TRUE;
			}
			return FALSE;
		}
		if (aacPacketType == 1)
		{
			return m_aac->Decode(bits, size);
		}
		return TRUE;
	}
	BOOL FLVFile::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		EVENT_AUDIO(data, size, audio);
		return TRUE;
	}
	BOOL FLVFile::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size)
	{
		EVENT_AUDIO(data, size, audio);
		return TRUE;
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
		size_t size = 0;
		for (;;)
		{
			UINT tagSize = 0;
			if (fread(&tagSize, sizeof(UINT), 1, m_hFile) <= 0)
				break;
			tagSize = htonl(tagSize);
			FLV_TAG_HEADER tag = { 0 };
			if (fread(&tag, sizeof(FLV_TAG_HEADER), 1, m_hFile) <= 0)
				break;
			INT size = Utility::ToINT24(tag.size);
			if (size > 0)
			{
				TinyScopedArray<BYTE> data(new BYTE[size]);
				if (fread(data, size, 1, m_hFile) <= 0)
					break;
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

	void FLVFile::OnAudioDone(BYTE* data, INT size, LPVOID ps)
	{
		EVENT_AUDIO(data, size, ps);
	}
	void FLVFile::OnVideoDone(BYTE* data, INT size, LPVOID ps)
	{
		EVENT_VIDEO(data, size, ps);
	}
}

