#pragma once
#include "Common.h"
#include "H264BitReader.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	class H264VideoConfig
	{
		DISALLOW_COPY_AND_ASSIGN(H264VideoConfig)
	public:
		H264VideoConfig();
		H264VideoConfig(VideoCodec codec,
			VideoCodecProfile profile,
			VideoPixelFormat format,
			INT frameRate,
			const TinySize& codedSize,
			const TinyRectangle& visibleRect,
			const TinySize& naturalSize);
		BOOL operator == (const H264VideoConfig& o);
		BOOL operator != (const H264VideoConfig& o);
	private:
		VideoPixelFormat	m_pixelFormat;
		VideoCodec			m_codec;
		VideoCodecProfile	m_profile;
		INT					m_frameRate;
		TinySize			m_codedSize;
		TinySize			m_naturalSize;
		TinyRectangle		m_visibleRect;
	};
	/// <summary>
	/// H264解析
	/// </summary>
	class H264Parser
	{
		DISALLOW_COPY_AND_ASSIGN(H264Parser)
	public:
		H264Parser();
		virtual ~H264Parser();
		const H264VideoConfig& GetVideoConfig() const;
		BOOL Parse(const BYTE* bits, LONG size);
		static BOOL FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code);//获得起始码
	private:
		BOOL ParseSPS(const BYTE* bits, LONG size, BYTE code, INT& spsID);
		BOOL ParsePPS(const BYTE* bits, LONG size, BYTE code, INT& ppsID);
		BOOL ParseSliceHeader(const BYTE* bits, LONG size, BYTE code);
		BOOL ReadUE(INT* val);
		BOOL ReadSE(INT* val);
		BOOL ParseSPSScalingLists(H264SPS& sps);
		BOOL ParseScalingList(INT size, INT* scaling_list, BOOL* bDefault);
		BOOL ParseVUIParameters(H264SPS& sps);
		BOOL ParsePPSScalingLists(const H264SPS& sps, H264PPS& pps);
		BOOL SetVideoConfig(const H264SPS* sps);
	private:
		const BYTE*				m_bits;
		LONG					m_size;
		LONG					m_count;
		TinyMap<INT, H264SPS>	m_mapsps;
		TinyMap<INT, H264PPS>	m_mappps;
		vector<BYTE>			m_sps;
		vector<BYTE>			m_pps;
		H264BitReader			m_reader;
		H264VideoConfig			m_lastConfig;
	};
}
