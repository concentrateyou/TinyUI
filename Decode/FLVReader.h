#pragma once
#include "Common.h"
#include "AACDecode.h"
#include "MPG123Decode.h"
#include "H264Decode.h"
#include <vector>

namespace Decode
{
	/// <summary>
	/// FLV¶ÁÈ¡Æ÷
	/// </summary>
	class FLVReader
	{
		DISALLOW_COPY_AND_ASSIGN(FLVReader)
	public:
		FLVReader();
		virtual ~FLVReader();
		FLV_SCRIPTDATA GetScript();
		BOOL OpenFile(LPCSTR pzFile);
		BOOL OpenURL(LPCSTR pzURL);
		BOOL ReadBlock(FLV_BLOCK& block);
		DWORD Seek(LONG offset, DWORD dwFlag);
		BOOL Close();
	private:
		BOOL ParseScript(BYTE* data, INT size, FLV_SCRIPTDATA& script);
		BOOL ParseVideo(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAudio(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, FLV_BLOCK& block);
	private:
		BOOL					m_bFirstAudio;
		BOOL					m_bFirstVideo;
		BOOL					m_bNetwork;
		BOOL					m_bAudio;
		BOOL					m_bVideo;
		LONGLONG				m_llFirst;
		BYTE					m_minusOne;
		FLV_SCRIPTDATA			m_script;
		ULONGLONG				m_offset;
		TinyComPtr<IStream>		m_stream;
		TinyBufferArray<BYTE>	m_nalus;
		FLV_TAG_VIDEO			m_videoTag;
		LONGLONG				m_timestamp;
		BYTE*					m_naluPtr;
		INT						m_naluOffset;
	};
}

