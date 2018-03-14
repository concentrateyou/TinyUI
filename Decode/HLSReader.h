#pragma once
#include "Common.h"
#include "FLVReader.h"
#include "TSWriter.h"
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
		BOOL OpenURL(LPCSTR pzURL);
	private:
		BOOL ParsePlaylist(const string& response);
	private:
		UINT32				m_duration;
		LONGLONG			m_sequence;//序号
		TinyString			m_szURL;
		TinyArray<string>	m_playlist;//切片列表
		TinyComPtr<IStream>	m_stream;
	};
}



