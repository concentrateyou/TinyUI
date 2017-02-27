#include "../stdafx.h"
#include "TinyModule.h"
#include "TinyString.h"
#include "TinyHook.h"
#include "TinyApplication.h"
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

namespace TinyUI
{
	BOOL IsExecutableAddress(LPVOID pAddress)
	{
		if (!pAddress)
			return FALSE;
		MEMORY_BASIC_INFORMATION mi;
		VirtualQuery(pAddress, &mi, sizeof(mi));
		return (mi.State == MEM_COMMIT && (mi.Protect & PAGE_EXECUTE_FLAGS));
	}
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

	}
	BOOL TinyDetour::Initialize(FARPROC lpSRC, FARPROC lpDST)
	{
		if (!IsExecutableAddress(lpSRC) || !IsExecutableAddress(lpDST))
			return FALSE;
		m_lpSRC = lpSRC;
		m_lpDST = lpDST;
		if (!VirtualProtect((LPVOID)m_lpSRC, JMP_SIZE, PAGE_EXECUTE_READWRITE, &m_dwProtect))
			return FALSE;
		memcpy(m_data, (const void*)m_lpSRC, JMP_SIZE);
		return TRUE;
	}
	BOOL TinyDetour::BeginDetour()
	{
		if (!IsExecutableAddress(m_lpSRC) || !IsExecutableAddress(m_lpDST))
			return FALSE;
		ULONG src = ULONG(m_lpSRC);
		ULONG dst = ULONG(m_lpDST);
		ULONG64 relative = 0;
		relative = dst - (src + JMP_SIZE);
		DWORD oldProtect;
		if (!VirtualProtect((LPVOID)m_lpSRC, JMP_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect))
			return FALSE;
		LPBYTE val = (LPBYTE)m_lpSRC;
		*val = 0xE9;
		*(DWORD*)(val + 1) = DWORD(relative);
		if (!FlushInstructionCache(GetCurrentProcess(), m_lpSRC, JMP_SIZE))
			return FALSE;
		m_bDetour = TRUE;
		return TRUE;
	}
	BOOL TinyDetour::EndDetour()
	{
		if (!m_bDetour) return FALSE;
		DWORD oldProtect;
		if (!VirtualProtect((LPVOID)m_lpSRC, JMP_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect))
			return FALSE;
		memcpy((void*)m_lpSRC, m_data, JMP_SIZE);
		if (!VirtualProtect((LPVOID)m_lpSRC, JMP_SIZE, m_dwProtect, &oldProtect))
			return FALSE;
		if (!FlushInstructionCache(GetCurrentProcess(), m_lpSRC, JMP_SIZE))
			return FALSE;
		m_bDetour = FALSE;
		return TRUE;
	}
	BOOL TinyDetour::IsValid() const
	{
		return m_bDetour;
	}
	FARPROC TinyDetour::GetOrig() const
	{
		return m_lpSRC;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyInlineHook::TinyInlineHook()
		:m_lpSRC(NULL),
		m_lpDST(NULL),
		m_dwPatch(0),
		m_dwTrampoline(0),
		m_dwProtect(0),
		m_bDetour(FALSE)
	{

	}
	TinyInlineHook::~TinyInlineHook()
	{

	}
	BOOL TinyInlineHook::Initialize(LPVOID lpSRC, LPVOID lpDST)
	{
		if (!IsExecutableAddress(lpSRC) || !IsExecutableAddress(lpDST))
			return FALSE;
		CALL_REL call = {
			0xE8,                   // E8 xxxxxxxx: CALL +5+xxxxxxxx
			0x00000000              // Relative destination address
		};
		JMP_REL jmp = {
			0xE9,                   // E9 xxxxxxxx: JMP +5+xxxxxxxx
			0x00000000              // Relative destination address
		};
		JCC_REL jcc = {
			0x0F, 0x80,             // 0F8* xxxxxxxx: J** +6+xxxxxxxx
			0x00000000              // Relative destination address
		};
		BYTE	trampoline[128] = { 0 };
		BYTE*	ps = trampoline;
		BOOL bBreak = FALSE;
		for (;;)
		{
			if (bBreak)
				break;
			HDE hs = { 0 };
			LPVOID lpAddress = (LPVOID)((ULONG_PTR)lpSRC + m_dwTrampoline);
			DWORD dwCopy = HDE_DISASM(lpAddress, &hs);
			ASSERT(dwCopy == hs.len);
			if (hs.flags & F_ERROR)
				return FALSE;
			do
			{
				if (((ULONG_PTR)lpAddress - (ULONG_PTR)lpSRC) >= sizeof(JMP_REL))
				{
					dwCopy = sizeof(jmp);//Ìø»ØÀ´
					m_dwPatch = dwCopy;
					bBreak = TRUE;
					break;
				}
				if (hs.opcode == 0xE8)//call
				{
					ULONG_PTR dest = (ULONG_PTR)lpAddress + hs.len + (INT32)hs.imm.imm32;
					//(UINT32)(dest - ((ULONG_PTR)lpDST + sizeof(call)));
					break;
				}
				if ((hs.opcode & 0xFD) == 0xE9) // jmp
				{
					break;
				}
				if (((hs.opcode & 0xF0) == 0x70) || (hs.opcode == 0xE3) || ((hs.opcode2 & 0xF0) == 0x80)) // jcc
				{
					break;
				}
				if (((hs.opcode & 0xFE) == 0xC2) || // ret
					((hs.opcode & 0xFD) == 0xE9) || // jmp rel
					(((hs.modrm & 0xC7) == 0x05) && ((hs.opcode == 0xFF) && (hs.modrm_reg == 4))) || // jmp rip
					((hs.opcode == 0xFF) && (hs.opcode2 == 0x25))) // jmp abs
				{
					break;
				}
			} while (0);
			memcpy(ps, (LPVOID)lpAddress, hs.len);
			ps += hs.len;
			m_dwTrampoline += dwCopy;
		}
		m_trampoline.Reset(new BYTE[m_dwTrampoline]);
		memcpy(m_trampoline, trampoline, m_dwTrampoline);

		//DWORD dwOffset = m_dwTrampoline - m_dwPatch;

		//m_pBACK = trampoline + dwOffset;
		//*m_pBACK = 0xE9;
		//*(DWORD*)(m_pBACK + 1) = (reinterpret_cast<DWORD>(m_trampoline) - reinterpret_cast<DWORD>(m_pBACK) - m_dwPatch);

		return TRUE;
	}
	BOOL TinyInlineHook::BeginDetour()
	{
		DWORD oldProtect = 0;
		if (!VirtualProtect((LPVOID)m_trampoline, m_dwPatch, PAGE_EXECUTE_READWRITE, &oldProtect))
			return FALSE;
		DWORD src = DWORD(m_lpSRC);
		DWORD dst = DWORD(m_lpDST);
		DWORD relative = 0;
		relative = dst - (src + m_dwPatch);
		LPBYTE val = (LPBYTE)m_trampoline;
		*val = 0xE9;
		*(DWORD*)(val + 1) = DWORD(relative);
		if (!FlushInstructionCache(GetCurrentProcess(), m_lpSRC, JMP_SIZE))
			return FALSE;
	}
	BOOL TinyInlineHook::EndDetour()
	{
		return TRUE;
	}
	BOOL TinyInlineHook::IsValid() const
	{
		return m_bDetour;
	}
	LPVOID TinyInlineHook::GetOrig()
	{
		return reinterpret_cast<LPVOID>(m_trampoline.Ptr());
	}
}