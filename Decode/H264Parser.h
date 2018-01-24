#pragma once
#include "Common.h"
#include "H264BitReader.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	class H264VideoDecoderConfig
	{
		DISALLOW_COPY_AND_ASSIGN(H264VideoDecoderConfig)
	public:
		H264VideoDecoderConfig();
		H264VideoDecoderConfig(VideoCodec codec,
			VideoCodecProfile profile,
			VideoPixelFormat format,
			const TinySize& videoSize,
			const TinyRectangle& visibleRect,
			const TinySize& naturalSize);
		BOOL operator == (const H264VideoDecoderConfig& o);
		BOOL operator != (const H264VideoDecoderConfig& o);
	private:
		VideoPixelFormat	m_pixelFormat;
		VideoCodec			m_codec;
		VideoCodecProfile	m_profile;
		TinySize			m_videoSize;
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
		const H264VideoDecoderConfig& GetVideoDecoderConfig() const;
		BOOL Parse(const BYTE* bits, LONG size);
		static BOOL FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code);//获得起始码
	private:
		BOOL ParseSPS(const BYTE* bits, LONG size, BYTE code, INT& spsID);
		BOOL ParsePPS(const BYTE* bits, LONG size, BYTE code, INT& ppsID);
		BOOL ParseSlice(const BYTE* bits, LONG size, BYTE code);
		BOOL ReadUE(INT* val);
		BOOL ReadSE(INT* val);
		BOOL ParseSPSScalingLists(H264SPS& sps);
		BOOL ParseScalingList(INT size, INT* scaling_list, BOOL* bDefault);
		BOOL ParseVUIParameters(H264SPS& sps);
		BOOL ParsePPSScalingLists(const H264SPS& sps, H264PPS& pps);
		BOOL SetVideoDecoderConfig(const H264SPS* sps);
	private:
		const BYTE*				m_bits;
		LONG					m_size;
		LONG					m_count;
		TinyMap<INT, H264SPS>	m_mapsps;
		TinyMap<INT, H264PPS>	m_mappps;
		vector<BYTE>			m_sps;
		vector<BYTE>			m_pps;
		H264BitReader			m_reader;
		H264VideoDecoderConfig	m_lastConfig;
	};
}
