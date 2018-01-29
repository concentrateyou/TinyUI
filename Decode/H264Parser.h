#pragma once
#include "Common.h"
#include "H264BitReader.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// H264解析
	/// </summary>
	class H264Parser
	{
		DISALLOW_COPY_AND_ASSIGN(H264Parser)
	public:
		H264Parser(ConfigCallback& callback);
		virtual ~H264Parser();
		const H264VideoConfig& GetVideoConfig() const;
		BOOL Parse(const BYTE* bits, LONG size, H264NALU& nalu);
		static BOOL FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code);//获得起始码
	private:
		BOOL ParseSPS(const BYTE* bits, LONG size, BYTE code, INT& spsID);
		BOOL ParsePPS(const BYTE* bits, LONG size, BYTE code, INT& ppsID);
		BOOL ParseSliceHeader(const BYTE* bits, LONG size, BYTE code, H264NALU& s);
		BOOL ReadUE(INT* val);
		BOOL ReadSE(INT* val);
		BOOL ParseSPSScalingLists(H264SPS& sps);
		BOOL ParseScalingList(INT size, INT* scaling_list, BOOL* bDefault);
		BOOL ParseVUIParameters(H264SPS& sps);
		BOOL ParsePPSScalingLists(const H264SPS& sps, H264PPS& pps);
		BOOL SetVideoConfig(const H264SPS* sps, BYTE code);
	private:
		ConfigCallback			m_callback;
		TinyMap<INT, H264SPS>	m_spsMap;
		TinyMap<INT, H264PPS>	m_ppsMap;
		vector<BYTE>			m_sps;
		vector<BYTE>			m_pps;
		H264BitReader			m_reader;
		H264VideoConfig			m_lastConfig;
	};
}
