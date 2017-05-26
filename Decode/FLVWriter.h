#pragma once
#include "Common.h"
#include "AACDecode.h"
#include "MPG123Decode.h"
#include "H264Decode.h"
#include <vector>

namespace Decode
{
#define SAVC(x)    static const AVal av_##x = AVC(#x)
	/// <summary>
	/// FLV目前支持支AAC+H264
	/// </summary>
	class FLVWriter
	{
		DISALLOW_COPY_AND_ASSIGN(FLVWriter)
	public:
		FLVWriter();
		virtual ~FLVWriter();
		BOOL Create(LPCSTR pzFile);
		BOOL WriteScriptTag(FLV_SCRIPTDATA& script);
		BOOL WriteAudioTag(FLV_PACKET& packet, BYTE* bits, LONG size);
		BOOL WriteVideoTag(FLV_PACKET& packet, BYTE* bits, LONG size);
		BOOL Close();
	private:
		TinyComPtr<IStream>		m_stream;
	};
}

