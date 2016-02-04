#pragma once
#include "TinyCommon.h"
#include "TinySingle.h"
#include "TinyString.h"

namespace TinyUI
{
	struct EXCLUDEDMODULE
	{
		HMODULE		hModule;
		CHAR		pzModule[MAX_PATH];
	};
	class TinyAPIFunction;
	/// <summary>
	/// API¹³×Ó
	/// </summary>
	class TinyAPIHook
	{
	public:
		TinyAPIHook();
		virtual ~TinyAPIHook();
		BOOL Add(LPCSTR pszCalleeModName, LPCSTR pszFunctionName, PROC  pfnOrig, PROC  pfnHook);
		BOOL Remove(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	public:
		static TinyAPIHook* GetInstance();
	private:
		static TinyAPIHook* m_pHook;
		TinyArray<TinyAPIFunction*> m_hookFs;
		TinyArray<EXCLUDEDMODULE>	m_excludes;
	};
	__declspec(selectany) TinyAPIHook* TinyAPIHook::m_pHook = NULL;
	/// <summary>
	/// APIº¯Êý
	/// </summary>
	class TinyAPIFunction
	{
		friend class TinyAPIHook;
	public:
		TinyAPIFunction(TinyAPIHook* pAPIHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook);
		~TinyAPIFunction();
		BOOL InstallHook();
		BOOL UninstallHook();
	private:
		BOOL DoInstallHook(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);
		BOOL DoInstallHook(HMODULE hmodCaller, LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);
	private:
		BOOL				m_bInstall;
		PROC				m_pfnOrig;
		PROC				m_pfnHook;
		CHAR				m_pzFunctionName[MAX_PATH];
		CHAR				m_pzCalleeModName[MAX_PATH];
		TinyLock			m_lock;
		TinyAPIHook*		m_pAPIHook;
		static PVOID        m_pMaximumApplicationAddress;
	};
	__declspec(selectany) PVOID TinyAPIFunction::m_pMaximumApplicationAddress = NULL;
}
