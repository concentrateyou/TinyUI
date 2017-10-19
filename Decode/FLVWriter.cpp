#include "stdafx.h"
#include "amf.h"
#include "FLVWriter.h"
#include "RTMPStream.h"

namespace Decode
{
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
	SAVC(fileSize);
	//10(mp4a)AAC
	//7(avc1)H.264
	//4 On2 VP6
	//2 Sorenson H.263
	SAVC(avc1);
	SAVC(mp4a);
	//////////////////////////////////////////////////////////////////////////
	FLVWriter::FLVWriter()
		:m_dwPreviousSize(0)
	{

	}
	FLVWriter::~FLVWriter()
	{

	}
	BOOL FLVWriter::Create(LPCSTR pzFile)
	{
		HRESULT hRes = SHCreateStreamOnFile(pzFile, STGM_CREATE | STGM_WRITE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		FLV_HEADER header = { 0 };
		header.signature[0] = 'F';
		header.signature[1] = 'L';
		header.signature[2] = 'V';
		header.streamType = 0x01 | 0x04;
		header.version = 0;
		header.offset[3] = 9;
		ULONG ls = 0;
		hRes = m_stream->Write(&header, sizeof(header), &ls);
		if (hRes != S_OK || ls != sizeof(header))
			return FALSE;
		//Tag 0
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		hRes = m_stream->Write(&dwPreviousSize, sizeof(dwPreviousSize), &ls);
		if (hRes != S_OK || ls != sizeof(dwPreviousSize))
			return FALSE;
		return TRUE;
	}
	BOOL FLVWriter::WriteScriptTag(FLV_SCRIPTDATA& script)
	{
		ULONG ls = 0;
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
		HRESULT hRes = m_stream->Write(&tag, sizeof(tag), &ls);
		if (hRes != S_OK || ls != sizeof(tag))
			return FALSE;
		hRes = m_stream->Write(bits, dwSize, &ls);
		if (hRes != S_OK || ls != dwSize)
			return FALSE;
		m_dwPreviousSize = sizeof(FLV_TAG_HEADER) + dwSize;
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		hRes = m_stream->Write(&dwPreviousSize, sizeof(dwPreviousSize), &ls);
		if (hRes != S_OK || ls != sizeof(dwPreviousSize))
			return FALSE;
		return TRUE;
	}
	BOOL FLVWriter::WriteAudioTag(FLV_PACKET& packet, BYTE* bits, LONG size)
	{
		ULONG ls = 0;
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
		HRESULT hRes = m_stream->Write(&tag, sizeof(tag), &ls);
		if (hRes != S_OK || ls != sizeof(tag))
			return FALSE;
		FLV_TAG_AUDIO audio = { 0 };
		audio.bitsPerSample = packet.bitsPerSample;
		audio.channel = packet.channel;
		audio.codeID = packet.codeID;
		audio.samplesPerSec = packet.samplesPerSec;
		hRes = m_stream->Write(&audio, sizeof(audio), &ls);
		if (hRes != S_OK || ls != sizeof(audio))
			return FALSE;
		hRes = m_stream->Write(&packet.packetType, sizeof(packet.packetType), &ls);
		if (hRes != S_OK || ls != sizeof(packet.packetType))
			return FALSE;
		hRes = m_stream->Write(bits, size, &ls);
		if (hRes != S_OK || ls != size)
			return FALSE;
		m_dwPreviousSize = sizeof(FLV_TAG_HEADER) + dwSize;
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		hRes = m_stream->Write(&dwPreviousSize, sizeof(dwPreviousSize), &ls);
		if (hRes != S_OK || ls != sizeof(dwPreviousSize))
			return FALSE;
		return TRUE;
	}
	BOOL FLVWriter::WriteVideoTag(FLV_PACKET& packet, BYTE* bits, LONG size)
	{
		ULONG ls = 0;
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
		HRESULT hRes = m_stream->Write(&tag, sizeof(tag), &ls);
		if (hRes != S_OK || ls != sizeof(tag))
			return FALSE;
		FLV_TAG_VIDEO video = { 0 };
		video.codeID = packet.codeID;
		video.codeType = packet.codeType;
		hRes = m_stream->Write(&video, sizeof(video), &ls);
		if (hRes != S_OK || ls != sizeof(video))
			return FALSE;
		hRes = m_stream->Write(&packet.packetType, sizeof(packet.packetType), &ls);
		if (hRes != S_OK || ls != sizeof(packet.packetType))
			return FALSE;
		BYTE cts[3];
		cts[0] = (packet.pts - packet.dts) >> 16 & 0xFF;
		cts[1] = (packet.pts - packet.dts) >> 8 & 0xFF;
		cts[2] = (packet.pts - packet.dts) & 0xFF;
		hRes = m_stream->Write(cts, 3, &ls);
		if (hRes != S_OK || ls != sizeof(cts))
			return FALSE;
		hRes = m_stream->Write(bits, size, &ls);
		if (hRes != S_OK || ls != size)
			return FALSE;
		m_dwPreviousSize = sizeof(FLV_TAG_HEADER) + dwSize;
		DWORD dwPreviousSize = ntohl(m_dwPreviousSize);
		hRes = m_stream->Write(&dwPreviousSize, sizeof(dwPreviousSize), &ls);
		if (hRes != S_OK || ls != sizeof(dwPreviousSize))
			return FALSE;
		return TRUE;
	}

	BOOL FLVWriter::WriteAACASC(BYTE* bits, LONG size)
	{
		FLV_PACKET audio = { 0 };
		audio.bitsPerSample = 3;
		audio.samplesPerSec = 1;
		audio.channel = 1;
		audio.codeID = FLV_CODECID_AAC;
		audio.packetType = 0;//AAC Sequence header
		return WriteAudioTag(audio, bits, size);
	}

	BOOL FLVWriter::WriteAACRaw(BYTE* bits, LONG size, LONGLONG timestamp)
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
		return WriteAudioTag(audio, bits, size);
	}

	BOOL FLVWriter::WriteH264AVC(const vector<BYTE>& sps, const vector<BYTE>& pps)
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
		return WriteVideoTag(video, &bits[0], bits.size());
	}

	BOOL FLVWriter::WriteH264NALU(BYTE frameType, BYTE* bits, LONG size, LONGLONG pts, LONGLONG dts)
	{
		FLV_PACKET video = { 0 };
		video.codeID = FLV_CODECID_H264;
		video.codeType = frameType;
		video.packetType = 1;//AVC NALU
		video.dts = dts;
		video.pts = pts;
		vector<BYTE> data;
		data.push_back((size >> 24) & 0xFF);
		data.push_back((size >> 16) & 0xFF);
		data.push_back((size >> 8) & 0xFF);
		data.push_back(size & 0xFF);
		data.insert(data.end(), bits, bits + size);
		return WriteVideoTag(video, &data[0], data.size());
	}
	BOOL FLVWriter::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

