#pragma once
#include "Utility.h"
using namespace TinyUI;

namespace Decode
{
	typedef struct tagFLV_HEADER
	{
		BYTE signature[3];
		BYTE version;
		BYTE streamType;
		BYTE offset[4];
	}FLV_HEADER;

	typedef struct tagFLV_TAG_HEADER
	{
		BYTE type;
		BYTE size[3];
		BYTE timestamp[3];
		BYTE timestampex;
		BYTE streamID[3];
	} FLV_TAG_HEADER;

	typedef struct tagFLV_TAG_VIDEO
	{
		BYTE codeID : 4;//±‡¬ÎID
		BYTE codeType : 4;//÷°¿‡–Õ
	}FLV_TAG_VIDEO;

	typedef struct tagFLV_TAG_AUDIO
	{
		BYTE channel : 1;
		BYTE bitsPerSample : 1;
		BYTE samplesPerSec : 2;
		BYTE codeID : 4;
	}FLV_TAG_AUDIO;

	enum FLV_CODECID
	{
		FLV_CODECID_PCM = 0,
		FLV_CODECID_ADPCM = 1,
		FLV_CODECID_MP3 = 2,
		FLV_CODECID_PCM_LE = 3,
		FLV_CODECID_NELLYMOSER_16KHZ_MONO = 4,
		FLV_CODECID_NELLYMOSER_8KHZ_MONO = 5,
		FLV_CODECID_NELLYMOSER = 6,
		FLV_CODECID_PCM_ALAW = 7,
		FLV_CODECID_PCM_MULAW = 8,
		FLV_CODECID_AAC = 10,
		FLV_CODECID_SPEEX = 11
	};

	class FLVDecode
	{
	public:
		FLVDecode(LPCSTR pzFile);
		~FLVDecode();
		BOOL Decode();
	private:
		void ParseVideo(BYTE* data, INT size);
		void ParseAudio(BYTE* data, INT size);
		void ParseScript(BYTE* data, INT size);
		void ParseAAC(BYTE* data, INT size);
		void ParseMP3(BYTE* data, INT size);
		void ParsePCM(BYTE* data, INT size);
	private:
		FLV_HEADER	m_header;
		FILE*		m_hFile;
		BOOL		m_bAudio;
		BOOL		m_bVideo;
	};
}

