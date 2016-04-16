#pragma once
#include "TinyCommon.h"
#include "TinySingle.h"
#include "TinyString.h"

namespace TinyUI
{
	class TinyIATFunction;
	class TinyIATHook;
	/// <summary>
	/// IAT注入
	/// </summary>
	class TinyIATHook
	{
	protected:
		typedef struct tagEXCLUDEDMODULE
		{
			HMODULE		hModule;
			CHAR		pzModule[MAX_PATH];
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
		static TinyIATHook* GetInstance();
		static PROC GetOriginalProc(LPCSTR pszCalleeModName, LPCSTR pszFunctionName);
	protected:
		static TinyIATHook* m_pHook;
		TinyArray<TinyScopedReferencePtr<TinyIATFunction>> m_hookFs;
		TinyArray<EXCLUDEDMODULE>	m_excludes;
	};
	SELECTANY TinyIATHook* TinyIATHook::m_pHook = NULL;
	/// <summary>
	/// API函数
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
		TinyIATHook*		m_pAPIHook;
		static PVOID        m_pMaximumApplicationAddress;
	};
	SELECTANY PVOID TinyIATFunction::m_pMaximumApplicationAddress = NULL;
	//////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 内联注入(inline),替换函数指令块以后实现貌似写的通用有点复杂
	/// </summary>
	class TinyInlineHook
	{
	public:
		TinyInlineHook(PROC pfnCurrent, PROC pfnNew);
		~TinyInlineHook();
		BOOL InstallHook();
		BOOL UninstallHook();
	private:
		PROC m_pfnOrig;
		PROC m_pfnNew;
	};
}
