#include "stdafx.h"
#include "HLSReader.h"

namespace Decode
{
	HLSReader::HLSReader()
	{
		m_numbers[0] = -1;
		m_numbers[1] = -1;
	}
	HLSReader::~HLSReader()
	{
	}
	BOOL HLSReader::Open(LPCSTR pzURL)
	{
		m_client.Close();
		if (!m_client.Open(pzURL))
			return FALSE;
		string response;
		if (!m_client.GetResponse().ReadAsString(response))
			return FALSE;
		if (!ParsePlaylist1(response))
			return FALSE;
		if (m_playlist.IsEmpty())
			return FALSE;
		m_szURL = std::move(m_playlist[0]);//默认取第一个
		m_client.Close();
		if (!m_client.Open(m_szURL.c_str()))
			return FALSE;
		if (!m_client.GetResponse().ReadAsString(response))
			return FALSE;
		if (!ParsePlaylist2(response, TRUE))
			return FALSE;
		return TRUE;
	}
	void HLSReader::Close()
	{
		m_client.Close();
	}
	BOOL HLSReader::GetSegment(Segment& segment)
	{
		TinyAutoLock lock(m_lock);
		ITERATOR s = m_segments.First();
		if (s != NULL)
		{
			segment = m_segments.GetAt(s);
			return TRUE;
		}
		return FALSE;
	}
	BOOL HLSReader::ParsePlaylist1(const string& response)
	{
		TinyStringReader reader(response);
		CHAR* line1 = NULL;
		CHAR* line2 = NULL;
		if (!reader.ReadLine(line1, line2))
			return FALSE;
		if (strncasecmp(line1, "#EXTM3U", 7) != 0)
			return FALSE;
		BOOL bSTREAMINF = FALSE;
		for (;;)
		{
			if (!reader.ReadLine(line1, line2))
				break;
			if (strncasecmp(line1, "#EXT-X-ENDLIST", 14) == 0)
				break;
			do
			{
				if (strncasecmp(line1, "#EXT-X-STREAM-INF:", 18) == 0)
				{
					string s(line1, 18, line2 - line1 - 18);
					if (!s.empty())
					{
						vector<string> vals;
						SplitString(s, ',', &vals);
						bSTREAMINF = TRUE;
					}
					break;
				}
				if (strncasecmp(line1, "#EXT-X-MEDIA-SEQUENCE:", 22) == 0)
				{
					LONGLONG s = strtoll(line1 + 22, NULL, 10);
					if (m_numbers[0] == -1)
					{
						m_numbers[0] = s;
					}
					else
					{
						if ((m_numbers[0] + 1) != s)
						{
							//更新TODO
						}
					}
					break;
				}
				if (line1[0])
				{
					if (bSTREAMINF)//解析第一级
					{
						m_playlist.Add(string(line1, 0, line2 - line1));
						bSTREAMINF = FALSE;
					}
					break;
				}
			} while (0);
		}
		return TRUE;
	}
	BOOL HLSReader::ParsePlaylist2(const string& response, BOOL bF)
	{
		TinyStringReader reader(response);
		CHAR* line1 = NULL;
		CHAR* line2 = NULL;
		if (!reader.ReadLine(line1, line2))
			return FALSE;
		if (strncasecmp(line1, "#EXTM3U", 7) != 0)
			return FALSE;
		BOOL bEXTINF = FALSE;
		BOOL bUPDATE = FALSE;
		TinyLinkList<Segment> segments;
		Segment segment;
		for (;;)
		{
			if (!reader.ReadLine(line1, line2))
				break;
			if (strncasecmp(line1, "#EXT-X-ENDLIST", 14) == 0)
				break;
			do
			{
				if (strncasecmp(line1, "#EXTINF:", 8) == 0)
				{
					segment.Time = strtod(line1 + 8, NULL);
					bEXTINF = TRUE;
					break;
				}
				if (strncasecmp(line1, "#EXT-X-MEDIA-SEQUENCE:", 22) == 0)
				{
					LONGLONG s = strtoll(line1 + 22, NULL, 10);
					if (m_numbers[1] == -1)
					{
						m_numbers[1] = s;
					}
					else
					{
						if ((m_numbers[1] + 1) == s)
						{
							bUPDATE = TRUE;
						}
						m_numbers[1] = s;
					}
					break;
				}
				if (line1[0])
				{
					if (bEXTINF)//解析第二级
					{
						string szURL = m_szURL;
						string::size_type pos = m_szURL.find_last_of('/');
						if (pos != string::npos)
						{
							szURL = m_szURL.substr(0, pos + 1);
						}
						szURL.append(line1, 0, line2 - line1);
						segment.File = std::move(szURL);
						segments.InsertLast(segment);
						bEXTINF = FALSE;
					}
					break;
				}
			} while (0);
		}
		if (bF)
		{
			TinyAutoLock lock(m_lock);
			m_segments.RemoveAll();
			m_segments.Append(&segments);//更新当前的切片
		}
		else
		{
			if (bUPDATE)
			{
				TinyAutoLock lock(m_lock);
				m_segments.RemoveAll();
				m_segments.Append(&segments);//更新当前的切片
			}
		}
		return TRUE;
	}
	BOOL HLSReader::ReadSegments()
	{
		if (m_szURL.empty())
			return FALSE;
		m_client.Close();
		if (!m_client.Open(m_szURL.c_str()))
			return FALSE;
		string response;
		if (!m_client.GetResponse().ReadAsString(response))
			return FALSE;
		if (!ParsePlaylist2(response, FALSE))
			return FALSE;
		return TRUE;
	}
}
