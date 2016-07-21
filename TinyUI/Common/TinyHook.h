#pragma once
#include "TinyCommon.h"
#include "TinySemaphore.h"
#include "TinyString.h"

namespace TinyUI
{
	class TinyIATFunction;
	class TinyIATHook;
	/// <summary>
	/// IAT×¢Èë
	/// </summary>
	class TinyIATHook : public TinyReference < TinyIATHook >
	{
	protected:
		typedef struct tagEXCLUDEDMODULE
		{
			HMODULE		hModule;
			TCHAR		pzModule[MAX_PATH];
		}EXCLUDEDMODULE;
	public:
		TinyIATHook();
		virtual ~TinyIATHook();
	public:
		BOOL Add(LPCSTR pszCalleeModName, LPCSTR pszFunctionName, PROC  pfnOrig, PROC  pfnHook);
		BOOL Remove(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
		void RemoveAll();
		BOOL IsModuleExclude(HMODULE hModule);
		void ExcludeModule(LPCTSTR lpszModule);
		TinyArray<HMODULE> GetExcludeModules();
		TinyIATFunction* GetFunction(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	public:
		static PROC GetOriginalProc(TinyIATHook* pHook, LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	protected:
		TinyArray<TinyScopedReferencePtr<TinyIATFunction>> m_hookFs;
		TinyArray<EXCLUDEDMODULE>	m_excludes;
	};
	/// <summary>
	/// APIº¯Êý
	/// </summary>
	class TinyIATFunction : public TinyReference < TinyIATFunction >
	{
		friend class TinyIATHook;
	public:
		TinyIATFunction(TinyIATHook* pAPIHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook);
		~TinyIATFunction();
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
		TinyIATHook*		m_pIATHook;
		static PVOID        m_pMaximumApplicationAddress;
	};
	SELECTANY PVOID TinyIATFunction::m_pMaximumApplicationAddress = NULL;
}
