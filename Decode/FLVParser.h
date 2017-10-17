#pragma once
#include "Common.h"
#include "AACDecode.h"
#include "x264Decode.h"
#include <vector>

using namespace std;
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// FLV大端数据
	/// 目前音频支持支AAC,MP3和PCM,视频H.264
	/// </summary>
	class FLVParser
	{
		DISALLOW_COPY_AND_ASSIGN(FLVParser)
	public:
		FLVParser();
		~FLVParser();
		BOOL Open(LPCSTR pzFile);
		BOOL Parse();
		BOOL Close();
	public:
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_AUDIO;
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_VIDEO;
		Event<void(FLV_SCRIPTDATA*)> EVENT_SCRIPT;
	private:
		BOOL ParseVideo(BYTE* data, INT size);
		BOOL ParseAudio(BYTE* data, INT size);
		BOOL ParseScript(BYTE* data, INT size);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, INT* cts, BYTE* data, INT size);
	private:
		BOOL							m_bAudio;
		BOOL							m_bVideo;
		BYTE							m_minusOne;
		FILE*							m_hFile;
		LONGLONG						m_timestamp;
		LONG							m_index;
		LONG							m_duration;
		FILE*							m_h264File;
	};
}

