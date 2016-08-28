#include "../stdafx.h"
#include "TinyModule.h"
#include "TinyString.h"
#include "TinyHook.h"
#include "TinyApplication.h"
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

namespace TinyUI
{
	TinyIATHook::TinyIATHook()
	{
	}
	TinyIATHook::~TinyIATHook()
	{

	}
	BOOL TinyIATHook::Add(LPCSTR pszCalleeModName, LPCSTR pszFunctionName, PROC  pfnOrig, PROC  pfnHook)
	{
		TinyIATFunction* ps = new TinyIATFunction(this, pszCalleeModName, pszFunctionName, pfnOrig, pfnHook);
		if (ps && ps->InstallHook())
		{
			return m_hookFs.Add(ps);
		}
		return FALSE;
	}
	BOOL TinyIATHook::Remove(LPCSTR pszCalleeModName, LPCSTR pszFunctionName)
	{
		DWORD size = m_hookFs.GetSize();
		for (UINT i = 0; i < size; i++)
		{
			TinyScopedReferencePtr<TinyIATFunction>& fs = m_hookFs[i];
			if (strcasecmp(fs->m_pzCalleeModName, pszCalleeModName) == 0
				&& strcasecmp(fs->m_pzFunctionName, pszFunctionName) == 0)
			{
				if (fs->UninstallHook())
				{
					m_hookFs.Remove(fs);
					return TRUE;
				}

			}
		}
		return FALSE;
	}
	void TinyIATHook::RemoveAll()
	{
		DWORD size = m_hookFs.GetSize();
		for (DWORD i = 0; i < size; i++)
		{
			TinyScopedReferencePtr<TinyIATFunction>& fs = m_hookFs[i];
			fs->UninstallHook();
		}
		m_hookFs.RemoveAll();
	}
	BOOL TinyIATHook::IsModuleExclude(HMODULE hModule)
	{
		DWORD size = m_excludes.GetSize();
		for (DWORD i = 0; i < size; i++)
		{
			if (m_excludes[i].hModule == hModule)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	void TinyIATHook::ExcludeModule(LPCTSTR lpszModule)
	{
		EXCLUDEDMODULE em;
		memset(&em, 0, sizeof(EXCLUDEDMODULE));
		em.hModule = NULL;
#pragma warning(push)
#pragma warning(disable : 4996)
		strcpy((char*)lpszModule, em.pzModule);
		m_excludes.Add(em);
#pragma warning(pop)
	}
	TinyArray<HMODULE> TinyIATHook::GetExcludeModules()
	{
		TinyArray<HMODULE> vals;
		DWORD size = m_excludes.GetSize();
		for (DWORD i = 0; i < size; i++)
		{
			vals.Add(m_excludes[i].hModule);
		}
		return vals;
	}
	TinyIATFunction* TinyIATHook::GetFunction(LPCSTR pszCalleeModName, LPCSTR pszFunctionName)
	{
		DWORD size = m_hookFs.GetSize();
		for (DWORD i = 0; i < size; i++)
		{
			TinyScopedReferencePtr<TinyIATFunction>& fs = m_hookFs[i];
			if (strcasecmp(fs->m_pzCalleeModName, pszCalleeModName) == 0
				&& strcasecmp(fs->m_pzFunctionName, pszFunctionName) == 0)
			{
				return fs;
			}
		}
		return NULL;
	}
	PROC TinyIATHook::GetOriginalProc(TinyIATHook* pIATHook, LPCSTR pszCalleeModName, LPCSTR pszFunctionName)
	{
		TinyIATFunction* fs = pIATHook->GetFunction(pszCalleeModName, pszFunctionName);
		return fs ? fs->m_pfnOrig : NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyIATFunction::TinyIATFunction(TinyIATHook* pIATHook, LPCSTR  pszCalleeModName, LPCSTR pszFunctionName, PROC pfnOrig, PROC pfnHook)
		: m_pIATHook(pIATHook),
		m_pfnOrig(pfnOrig),
		m_pfnHook(pfnHook),
		m_bInstall(FALSE)
	{
		memset(m_pzFunctionName, 0, MAX_PATH);
		strcpy_s(m_pzFunctionName, MAX_PATH, pszFunctionName);
		memset(m_pzCalleeModName, 0, MAX_PATH);
		strcpy_s(m_pzCalleeModName, MAX_PATH, pszCalleeModName);

		if (!m_pMaximumApplicationAddress)
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			m_pMaximumApplicationAddress = si.lpMaximumApplicationAddress;
		}
		if (m_pfnOrig > m_pMaximumApplicationAddress)
		{
			PBYTE pfn = (PBYTE)m_pfnOrig;
			if (pfn[0] == 0x68)
			{
				PVOID pv = *(PVOID*)&pfn[1];
				m_pfnOrig = (PROC)pv;
			}
		}
	}
	TinyIATFunction::~TinyIATFunction()
	{

	}
	BOOL TinyIATFunction::DoInstallHook(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew)
	{
		if (!pfnCurrent || !pfnNew) return FALSE;
		TinyToolHelpEnumModule tools(GetCurrentProcessId());
		tools.Initialize();
		BOOL bRes = FALSE;
		DWORD dwSize = tools.GetSize();
		for (DWORD i = 0; i < dwSize; i++)
		{
			HMODULE hModule = tools[i];
			if (!m_pIATHook->IsModuleExclude(hModule))
			{
				if (DoInstallHook(hModule, pszCalleeModName, pfnCurrent, pfnNew))
				{
					bRes = TRUE;
				}
			}
		}
		return bRes;
	}

	BOOL TinyIATFunction::DoInstallHook(HMODULE hmodCaller, LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew)
	{
		if (!pfnCurrent || !pfnNew) return FALSE;
		TinyAutoLock lock(m_lock);
		try
		{
			ULONG size;
			PIMAGE_IMPORT_DESCRIPTOR ps = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
				hmodCaller,
				TRUE,
				IMAGE_DIRECTORY_ENTRY_IMPORT,
				&size);
			if (!ps) return FALSE;
			while (ps != NULL)
			{
				while (ps->Name)
				{
					LPCSTR lpszName = ((LPCSTR)((PBYTE)hmodCaller + ps->Name));
					if (strcasecmp(lpszName, pszCalleeModName) == 0)
						break;
					ps++;
				}
				if (ps->Name == 0) return FALSE;
				PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hmodCaller + (UINT_PTR)ps->FirstThunk);
				while (pThunk->u1.Function)
				{
					PROC* ppfn = (PROC*)&pThunk->u1.Function;
					BOOL bF = (*ppfn == pfnCurrent);
					if (!bF && (*ppfn > m_pMaximumApplicationAddress))
					{
						PBYTE pfn = (PBYTE)*ppfn;
						if (pfn[0] == 0x68)
						{
							ppfn = (PROC*)&pfn[1];
							bF = (*ppfn == pfnCurrent);
						}
					}
					if (bF)
					{
						MEMORY_BASIC_INFORMATION mbi;
						::VirtualQuery(ppfn, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
						if (!::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
						{
							return FALSE;
						}
						*ppfn = *pfnNew;
						DWORD dwOldProtect;
						::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
						return TRUE;
					}
					pThunk++;
				}
				ps++;
			}
		}
		catch (...)
		{

		}
		return FALSE;
	}

	BOOL TinyIATFunction::InstallHook()
	{
		return m_bInstall = DoInstallHook(m_pzCalleeModName, m_pfnOrig, m_pfnHook);
	}

	BOOL TinyIATFunction::UninstallHook()
	{
		if (m_bInstall)
		{
			DoInstallHook(m_pzCalleeModName, m_pfnHook, m_pfnOrig);
			m_bInstall = FALSE;
		}
		return !m_bInstall;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyDetour::TinyDetour()
		:m_bDetour(FALSE)
	{

	}
	TinyDetour::~TinyDetour()
	{
		if (IsValid())
		{
			EndDetour();
		}
	}
	BOOL TinyDetour::Initialize(FARPROC pfnOrig, FARPROC pfnNew)
	{
		if (!pfnOrig || !pfnNew)
			return FALSE;
		m_pfnOrig = pfnOrig;
		m_pfnNew = pfnNew;
		if (!VirtualProtect((LPVOID)m_pfnOrig, JMP_32_SIZE, PAGE_EXECUTE_READWRITE, &m_dwOrigProtect))
			return FALSE;
		memcpy(m_data, (const void*)m_pfnOrig, JMP_32_SIZE);
		return TRUE;
	}
	BOOL TinyDetour::BeginDetour()
	{
		if (!m_pfnOrig || !m_pfnNew)
			return FALSE;
		ULONG start = ULONG(m_pfnOrig);
		ULONG target = ULONG(m_pfnNew);
		ULONG64 offset = 0;
		ULONG64 diff = 0;
		//http://www.cnblogs.com/zhangdongsheng/archive/2012/12/06/2804234.html
		//计算偏移量(JMP的地址–代码地址–5 = 机器码跳转地址 x86)
		offset = target - (start + JMP_32_SIZE);
		DWORD oldProtect;
		VirtualProtect((LPVOID)m_pfnOrig, JMP_32_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
		LPBYTE ps = (LPBYTE)m_pfnOrig;
		*ps = 0xE9;
		*(DWORD*)(ps + 1) = DWORD(offset);
		m_bDetour = TRUE;
		return TRUE;
	}
	BOOL TinyDetour::EndDetour()
	{
		if (!m_bDetour) return FALSE;
		DWORD oldProtect;
		VirtualProtect((LPVOID)m_pfnOrig, JMP_32_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy((void*)m_pfnOrig, m_data, JMP_32_SIZE);
		VirtualProtect((LPVOID)m_pfnOrig, JMP_32_SIZE, m_dwOrigProtect, &oldProtect);
		m_bDetour = FALSE;
		return TRUE;
	}
	BOOL TinyDetour::IsValid() const
	{
		return m_bDetour;
	}
	FARPROC TinyDetour::GetOrig() const
	{
		return m_pfnOrig;
	}
}