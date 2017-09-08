#pragma once
#include "MShowCommon.h"

namespace MShow
{
	class MAppConfig
	{
	public:
		MAppConfig();
		~MAppConfig();
	public:
		string GetSaveFile();
		vector<string>& GetIPList();
	private:
		string GetValue(LPCSTR pzApp, LPCSTR pzKey);
		BOOL SetValue(LPCSTR pzApp, LPCSTR pzKey, LPCSTR pzValue);
	private:
		string m_szFile;
		vector<string>	m_ips;
	};
}


