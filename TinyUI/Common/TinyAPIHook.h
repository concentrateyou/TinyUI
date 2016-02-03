#pragma once
#include "TinyCommon.h"
#include "TinySingle.h"
#include "TinyString.h"

namespace TinyUI
{
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
		BOOL Remove();
	public:
		static TinyAPIHook* GetInstance();
	private:
		static TinyAPIHook* m_pHook;
		TinyArray<TinyAPIFunction*> m_hookFs;
	};
	__declspec(selectany) TinyAPIHook* TinyAPIHook::m_pHook = NULL;
	/// <summary>
	/// APIº¯Êý
	/// </summary>
	class TinyAPIFunction
	{
		friend class TinyAPIHook;
	public:
		TinyAPIFunction(TinyAPIHook* pHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook);
		~TinyAPIFunction();
		BOOL InstallHook();
		BOOL UninstallHook();
	private:
		void DoInstallHook(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);
	private:
		PROC				m_pfnOrig;
		PROC				m_pfnHook;
		TinyString			m_strFunctionName;
		TinyString			m_strCalleeModName;
		TinyAPIHook*		m_pAPIHook;
	};
}
