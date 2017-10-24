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
		string GetPrefix();//前端服务器器接口前缀
		string GetSaveFile();
		string GetIP();
		string GetDispatch();
	private:
		string GetValue(LPCSTR pzApp, LPCSTR pzKey);
		BOOL SetValue(LPCSTR pzApp, LPCSTR pzKey, LPCSTR pzValue);
	private:
		string m_szFile;
	};
}


