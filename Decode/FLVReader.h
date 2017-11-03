#pragma once
#include "Common.h"
#include "AACDecode.h"
#include "MPG123Decode.h"
#include "x264Decode.h"
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
		BOOL		OpenFile(LPCSTR pzFile);
		BOOL		OpenURL(LPCSTR pzURL);
		BOOL		ReadBlock(FLV_BLOCK& block);
		DWORD		Seek(LONG offset, DWORD dwFlag);
		BOOL		Close();
		LONGLONG	GetBasePTS();
	private:
		BOOL ParseScript(BYTE* data, INT size, FLV_SCRIPTDATA& script);
		BOOL ParseVideo(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAudio(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseNALUS(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
	private:
		BOOL					m_bNetwork;
		BOOL					m_bAudio;
		BOOL					m_bVideo;
		BYTE					m_minusOne;
		LONGLONG				m_count;
		ULONGLONG				m_offset;
		LONGLONG				m_basePTS;
		LONGLONG				m_timestamp;
		FLV_TAG_VIDEO			m_videoTag;
		FLV_SCRIPTDATA			m_script;
		TinyComPtr<IStream>		m_stream;
		
	};
}

