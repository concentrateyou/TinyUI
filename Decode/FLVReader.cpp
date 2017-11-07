#include "stdafx.h"
#include "amf.h"
#include "FLVReader.h"
#include "RTMPStream.h"
#include "HTTPStream.h"
#include "Common/TinyLogging.h"

namespace Decode
{
	FLVReader::FLVReader()
		:m_bAudio(FALSE),
		m_bVideo(FALSE),
		m_bNetwork(FALSE),
		m_minusOne(0),
		m_offset(0),
		m_timestamp(0),
		m_count(0),
		m_basePTS(-1)
	{

	}
	FLVReader::~FLVReader()
	{

	}
	FLV_SCRIPTDATA FLVReader::GetScript()
	{
		return m_script;
	}
	BOOL FLVReader::OpenURL(LPCSTR pzURL)
	{
		if (!pzURL)
			return FALSE;
		m_bNetwork = TRUE;
		TinyString szURL = pzURL;
		if (szURL.IndexOf("http://") != -1)
		{
			m_stream.Attach(new HTTPStream());
			if (!m_stream)
				return FALSE;
			HTTPStream* ps = static_cast<HTTPStream*>(m_stream.Ptr());
			ps->SetErrorCallback(BindCallback(&FLVReader::OnError, this));
			if (!ps->Open(pzURL))
				return FALSE;
		}
		if (szURL.IndexOf("rtmp://") != -1)
		{
			m_stream.Attach(new RTMPStream());
			if (!m_stream)
				return FALSE;
			RTMPStream* ps = static_cast<RTMPStream*>(m_stream.Ptr());
			if (!ps->Open(pzURL))
				return FALSE;
		}
		HRESULT hRes = S_OK;
		FLV_HEADER header = { 0 };
		ULONG ls = 0;
		hRes = m_stream->Read(&header, sizeof(FLV_HEADER), &ls);
		if (hRes != S_OK || ls != sizeof(FLV_HEADER))
			return FALSE;
		if (strncmp("FLV", (CHAR*)header.signature, 3) != 0)
			return FALSE;
		m_bAudio = (header.streamType & 0x04) != 0;
		m_bVideo = (header.streamType & 0x01) != 0;
		m_offset = ToINT32(header.offset);
		UINT tagSize = 0;
		hRes = m_stream->Read(&tagSize, sizeof(UINT), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		tagSize = htonl(tagSize);
		FLV_TAG_HEADER tag = { 0 };
		hRes = m_stream->Read(&tag, sizeof(FLV_TAG_HEADER), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		INT size = ToINT24(tag.size);
		if (size > 0)
		{
			TinyScopedArray<BYTE> data(new BYTE[size]);
			hRes = m_stream->Read(data, size, &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (tag.type == FLV_SCRIPT)
			{
				if (!ParseScript(data, size, m_script))
					return FALSE;
			}
		}
		return TRUE;
	}
	BOOL FLVReader::OpenFile(LPCSTR pzFile)
	{
		m_bNetwork = FALSE;
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		FLV_HEADER header = { 0 };
		ULONG ls = 0;
		hRes = m_stream->Read(&header, sizeof(FLV_HEADER), &ls);
		if (hRes != S_OK || ls != sizeof(FLV_HEADER))
			return FALSE;
		if (strncmp("FLV", (CHAR*)header.signature, 3) != 0)
			return FALSE;
		m_bAudio = (header.streamType & 0x04) != 0;
		m_bVideo = (header.streamType & 0x01) != 0;
		m_offset = ToINT32(header.offset);
		LARGE_INTEGER dlibMove = { 0 };
		dlibMove.QuadPart = m_offset;
		hRes = m_stream->Seek(dlibMove, STREAM_SEEK_SET, NULL);
		if (hRes != S_OK)
			return FALSE;
		UINT tagSize = 0;
		hRes = m_stream->Read(&tagSize, sizeof(UINT), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		tagSize = htonl(tagSize);
		FLV_TAG_HEADER tag = { 0 };
		hRes = m_stream->Read(&tag, sizeof(FLV_TAG_HEADER), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		INT size = ToINT24(tag.size);
		if (size > 0)
		{
			TinyScopedArray<BYTE> data(new BYTE[size]);
			hRes = m_stream->Read(data, size, &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (tag.type == FLV_SCRIPT)
			{
				if (!ParseScript(data, size, m_script))
					return FALSE;
				ULARGE_INTEGER libNew = { 0 };
				ZeroMemory(&dlibMove, sizeof(dlibMove));
				hRes = m_stream->Seek(dlibMove, STREAM_SEEK_CUR, &libNew);
				if (hRes != S_OK)
					return FALSE;
				m_offset = libNew.LowPart;
			}
		}
		return TRUE;
	}
	DWORD FLVReader::Seek(LONG offset, DWORD dwFlag)
	{
		LARGE_INTEGER dlibMove = { 0 };
		dlibMove.LowPart = offset;
		ULARGE_INTEGER libNew = { 0 };
		HRESULT hRes = m_stream->Seek(dlibMove, dwFlag, &libNew);
		if (hRes != S_OK)
			return FALSE;
		return libNew.LowPart;
	}
	BOOL FLVReader::ReadBlock(FLV_BLOCK& block)
	{
		UINT tagSize = 0;
		ULONG ls = 0;
		HRESULT hRes = m_stream->Read(&tagSize, sizeof(UINT), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		tagSize = htonl(tagSize);
		FLV_TAG_HEADER tag = { 0 };
		hRes = m_stream->Read(&tag, sizeof(FLV_TAG_HEADER), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		INT size = ToINT24(tag.size);
		if (size > 0)
		{
			block.type = tag.type;
			TinyScopedArray<BYTE> data(new BYTE[size]);
			hRes = m_stream->Read(data, size, &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (tag.type == FLV_AUDIO)
			{
				m_timestamp = static_cast<LONGLONG>(static_cast<UINT32>(ToINT24(tag.timestamp) | (tag.timestampex << 24)));
				LOG(ERROR) << "[FLVReader] Audio: " << m_timestamp;
				if (m_timestamp > 0)
				{
					if (m_count == 1 && m_basePTS == -1)
					{
						m_basePTS = m_timestamp;
					}
					if (m_basePTS != -1)
					{
						m_timestamp -= m_basePTS;
					}
				}
				return ParseAudio(data, size, block);
			}
			else if (tag.type == FLV_VIDEO)
			{
				m_timestamp = static_cast<LONGLONG>(static_cast<UINT32>(ToINT24(tag.timestamp) | (tag.timestampex << 24)));
				if (m_timestamp > 0)
				{
					if (m_count == 1 && m_basePTS == -1)
					{
						m_basePTS = m_timestamp;
					}
					if (m_basePTS != -1)
					{
						m_timestamp -= m_basePTS;
					}
				}
				return ParseVideo(data, size, block);
			}
		}
		return FALSE;
	}
	BOOL FLVReader::ParseVideo(BYTE* data, INT size, FLV_BLOCK& block)
	{
		BOOL bRes = FALSE;
		FLV_TAG_VIDEO* video = reinterpret_cast<FLV_TAG_VIDEO*>(data);
		m_videoTag = *video;
		switch (video->codeID)
		{
		case FLV_CODECID_H264:
			bRes = ParseH264(video, data + 1, size - 1, block);
			break;
		case FLV_CODECID_MPEG4:
			bRes = ParseMPEG4(video, data + 1, size - 1, block);
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
	BOOL FLVReader::ParseAudio(BYTE* data, INT size, FLV_BLOCK& block)
	{
		BOOL bRes = FALSE;
		//目前只支持MP3,AAC和PCM
		FLV_TAG_AUDIO* audio = reinterpret_cast<FLV_TAG_AUDIO*>(data);
		switch (audio->codeID)
		{
		case FLV_CODECID_PCM:
			bRes = ParsePCM(audio, data + 1, size - 1, block);
			break;
		case FLV_CODECID_AAC:
			bRes = ParseAAC(audio, data + 1, size - 1, block);
			break;
		case FLV_CODECID_MP3:
			bRes = ParseMP3(audio, data + 1, size - 1, block);
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
	BOOL FLVReader::ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block)
	{
		if (size < 2)
			return FALSE;
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		size -= 1;
		if (aacPacketType == 0)
		{
			block.dts = 0;
			block.pts = 0;
			block.audio.bitsPerSample = audio->bitsPerSample;
			block.audio.channel = audio->channel;
			block.audio.codeID = FLV_CODECID_AAC;
			block.audio.packetType = FLV_AudioSpecificConfig;
			block.audio.size = size;
			block.audio.data = new BYTE[size];
			memcpy(block.audio.data, bits, size);
		}
		if (aacPacketType == 1)
		{
			block.dts = m_timestamp;
			block.pts = m_timestamp;
			block.audio.bitsPerSample = audio->bitsPerSample;
			block.audio.channel = audio->channel;
			block.audio.codeID = FLV_CODECID_AAC;
			block.audio.packetType = FLV_AACRaw;
			block.audio.size = size;
			block.audio.data = new BYTE[size];
			memcpy(block.audio.data, bits, size);
			m_count++;
		}
		return TRUE;
	}
	BOOL FLVReader::ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block)
	{
		return TRUE;
	}
	BOOL FLVReader::ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block)
	{
		return TRUE;
	}
	BOOL FLVReader::ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block)
	{
		BYTE* bits = data;
		BYTE aacPacketType = *bits++;
		size -= 1;
		block.video.cts = (ToINT24(bits) + 0xFF800000) ^ 0xFF800000;
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
			block.dts = 0;
			block.pts = 0;
			block.video.codeID = video->codeID;
			block.video.codeType = video->codeType;
			block.video.packetType = FLV_AVCDecoderConfigurationRecord;
			block.video.size = buffer.GetSize();
			block.video.data = new BYTE[block.video.size];
			memcpy(block.video.data, buffer.GetPointer(), block.video.size);
		}
		if (aacPacketType == 1)
		{
			return ParseNALUS(video, bits, size, block);
		}
		return TRUE;
	}
	BOOL FLVReader::ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block)
	{
		return TRUE;
	}
	BOOL FLVReader::ParseNALUS(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block)
	{
		INT offsetNALU = 0;
		INT sizeofNALU = 0;
		BYTE* pNALU = data;
		for (;;)
		{
			switch (m_minusOne)
			{
			case 4:
			{
				sizeofNALU = ToINT32(pNALU);
				memcpy(pNALU, &H264StartCode, 4);
				offsetNALU += sizeofNALU + 4;
				pNALU += (sizeofNALU + 4);
			}
			break;
			case 3:
			{
				sizeofNALU = ToINT24(pNALU);
				memcpy(pNALU, &H264StartCode, 3);
				offsetNALU += sizeofNALU + 3;
				pNALU += (sizeofNALU + 3);
			}
			break;
			case 2:
			{
				sizeofNALU = ToINT16(pNALU);
				memcpy(pNALU, &H264StartCode, 4);
				offsetNALU += sizeofNALU + 2;
				pNALU += (sizeofNALU + 2);
			}
			break;
			case 1:
			{
				sizeofNALU = ToINT8(pNALU);
				memcpy(pNALU, &H264StartCode, 1);
				offsetNALU += sizeofNALU + 1;
				pNALU += (sizeofNALU + 1);
			}
			break;
			}
			if (offsetNALU >= size)
			{
				block.video.size = size;
				block.video.data = new BYTE[block.video.size];
				memcpy(block.video.data, data, size);
				block.video.codeID = video->codeID;
				block.video.codeType = video->codeType;
				block.video.packetType = FLV_NALU;
				block.dts = m_timestamp;
				block.pts = (block.dts + block.video.cts);
				break;
			}
		}
		m_count++;
		return TRUE;
	}
	BOOL FLVReader::ParseScript(BYTE* data, INT size, FLV_SCRIPTDATA& script)
	{
		AMFObject metaObj;
		if (AMF_Decode(&metaObj, reinterpret_cast<CHAR*>(data), size, FALSE) > 0)
		{
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
							if ("audiochannels" == val)
							{
								ASSERT(objProp->p_type == AMF_NUMBER);
								script.audiochannels = objProp->p_vu.p_number;
							}
						}
					}
				}
			}
			AMF_Reset(&metaObj);
			return TRUE;
		}
		return FALSE;
	}
	BOOL FLVReader::Close()
	{
		m_bNetwork = FALSE;
		m_bAudio = FALSE;
		m_bVideo = FALSE;
		m_minusOne = 0;
		m_offset = 0;
		m_timestamp = 0;
		m_basePTS = -1;
		m_stream.Release();
		ZeroMemory(&m_script, sizeof(m_script));
		return TRUE;
	}
	LONGLONG FLVReader::GetBasePTS()
	{
		return m_basePTS;
	}
	void FLVReader::OnError(INT iError)
	{
		if (!m_errorCallback.IsNull())
		{
			m_errorCallback(iError);
		}
	}
	void FLVReader::SetErrorCallback(Callback<void(INT)>&& callback)
	{
		m_errorCallback = std::move(callback);
	}
}

