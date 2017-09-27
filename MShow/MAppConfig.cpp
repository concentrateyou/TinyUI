#include "stdafx.h"
#include "MAppConfig.h"

namespace MShow
{
	string GetDefaultINIFile()
	{
		CHAR module[MAX_PATH];
		GetModuleFileName(NULL, module, MAX_PATH);
		string log(module);
		string::size_type backslash = log.rfind('\\', log.size());
		if (backslash != string::npos)
		{
			log.erase(backslash + 1);
		}
		log += TEXT("setting.ini");
		return log;
	}

	MAppConfig::MAppConfig()
	{
		m_szFile = GetDefaultINIFile();
	}

	MAppConfig::~MAppConfig()
	{
	}

	string MAppConfig::GetSaveFile()
	{
		return GetValue("Audio", "SaveFile");
	}
	string MAppConfig::GetPrefix()
	{
		return GetValue("Server", "Prefix");
	}
	vector<string>& MAppConfig::GetIPList()
	{
		string val = GetValue("Server", "IP");
		m_ips.clear();
		SplitString(val,',',&m_ips);
		return m_ips;
	}
	string MAppConfig::GetValue(LPCSTR pzApp, LPCSTR pzKey)
	{
		string value;
		value.resize(MAX_PATH + 1);
		if (GetPrivateProfileString(pzApp, pzKey, NULL, &value[0], MAX_PATH + 1, m_szFile.c_str()) == 0)
			return string();
		return value;
	}
	BOOL MAppConfig::SetValue(LPCSTR pzApp, LPCSTR pzKey, LPCSTR pzValue)
	{
		return WritePrivateProfileString(pzApp, pzKey, pzValue, m_szFile.c_str());
	}
}
