#pragma once
#include "Common.h"
#include "TSReader.h"
#include <vector>

using namespace std;
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// HLS解析
	/// https://tools.ietf.org/html/draft-pantos-http-live-streaming-10
	/// http://live.hkstv.hk.lxdns.com/live/hks/playlist.m3u8(测试源)
	/// </summary>
	class HLSReader
	{
	public:
		struct Segment
		{
			DOUBLE Time;
			string File;
		};
	public:
		DISALLOW_COPY_AND_ASSIGN(HLSReader)
	public:
		HLSReader();
		virtual ~HLSReader();
		BOOL Open(LPCSTR pzURL);
		BOOL ReadSegments();
		void Close();
		BOOL GetSegment(Segment& segment);
	private:
		BOOL ParsePlaylist1(const string& response);//第一级解析
		BOOL ParsePlaylist2(const string& response, BOOL bF = FALSE);//第二级解析
	private:
		BOOL					m_bBreak;
		UINT32					m_duration;
		LONGLONG				m_numbers[2];//主序号和次序号
		string					m_szURL;//当前的播放URL
		TinyLock				m_lock;
		TinyHTTPClient			m_client;
		TinyArray<string>		m_playlist;//播放列表
		TinyLinkList<Segment>	m_segments;//分片列表
	};
};

namespace TinyUI
{
	template<>
	class DefaultTraits < Decode::HLSReader::Segment >
	{
	public:
		static INT  Compare(const Decode::HLSReader::Segment& value1, const Decode::HLSReader::Segment& value2)
		{
			return (value1.File == value2.File) ? 0 : 1;
		}
	};
};


