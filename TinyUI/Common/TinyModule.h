#pragma once
#include "TinyObject.h"

namespace TinyUI
{
	/// <summary>
	/// 获得进程模块信息
	/// </summary>
	class TinyModuleList
	{
		typedef BOOL(WINAPI* PFNENUMPROCESSMODULES) (HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
		typedef BOOL(WINAPI* PFNGETMODULEINFORMATION) (HANDLE hProcess, HMODULE hModule, MODULEINFO* lpmodinfo, DWORD cb);
	public:
		TinyModuleList(DWORD dwProcessId);
		~TinyModuleList();
	};
};

