#include "stdafx.h"
#include "HLSReader.h"

namespace Decode
{
	HLSReader::HLSReader()
		:m_sequence(-1)
	{
	}

	HLSReader::~HLSReader()
	{
	}

	BOOL HLSReader::OpenURL(LPCSTR pzURL)
	{
		m_stream.Attach(new HTTPStream());
		if (!m_stream)
			return FALSE;
		HTTPStream* ps = static_cast<HTTPStream*>(m_stream.Ptr());
		if (!ps->Open(pzURL))
			return FALSE;
		string response;
		if (!ps->GetResponse().ReadAsString(response))
			return FALSE;
		if (!ParsePlaylist(response))
			return FALSE;
		m_szURL = pzURL;
		return TRUE;
	}

	BOOL HLSReader::ParsePlaylist(const string& response)
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
					m_sequence = strtoll(line1 + 22, NULL, 10);
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
}
