#include "../stdafx.h"
#include "TinyAPIHook.h"
#include "TinyModule.h"

namespace TinyUI
{
	TinyAPIHook::TinyAPIHook()
	{
	}
	TinyAPIHook::~TinyAPIHook()
	{

	}
	TinyAPIHook* TinyAPIHook::GetInstance()
	{
		if (m_pHook == NULL)
		{
			TinyLock lock;
			lock.Acquire();
			if (m_pHook == NULL)
			{
				m_pHook = new TinyAPIHook();
			}
			lock.Release();
		}
		return m_pHook;
	}
	BOOL TinyAPIHook::Add(LPCSTR pszCalleeModName, LPCSTR pszFunctionName, PROC  pfnOrig, PROC  pfnHook)
	{
		TinyAPIFunction* ps = new TinyAPIFunction(this, pszCalleeModName, pszFunctionName, pfnOrig, pfnHook);
		if (ps)
		{
			m_hookFs.Add(ps);
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyAPIHook::Remove()
	{
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyAPIFunction::TinyAPIFunction(TinyAPIHook* pHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook)
		: m_pAPIHook(pHook),
		m_pfnOrig(pfnOrig),
		m_pfnHook(pfnHook),
		m_strFunctionName(pszFunctionName),
		m_strCalleeModName(pszCalleeModName)
	{

	}

	void TinyAPIFunction::DoInstallHook(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew)
	{
		if (!pfnCurrent || !pfnNew) return;
		
	}

	BOOL TinyAPIFunction::InstallHook()
	{

		return TRUE;
	}

	BOOL TinyAPIFunction::UninstallHook()
	{
		return TRUE;
	}
}