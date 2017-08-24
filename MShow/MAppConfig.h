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
	private:
		string GetValue(LPCSTR pzApp, LPCSTR pzKey);
		BOOL SetValue(LPCSTR pzApp, LPCSTR pzKey, LPCSTR pzValue);
	private:
		string m_szFile;
	};
}


