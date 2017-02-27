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
		m_bDetour(FALSE)
	{

	}
	TinyInlineHook::~TinyInlineHook()
	{
		Uninitialize();
	}
	BOOL TinyInlineHook::Uninitialize()
	{
		BOOL bRes = TRUE;
		if (m_pTrampoline)
		{
			bRes &= VirtualFree(m_pTrampoline, 0, MEM_RELEASE);
			m_pTrampoline = NULL;
		}
		return bRes;
	}
	BOOL TinyInlineHook::Initialize(LPVOID lpSRC, LPVOID lpDST)
	{
		if (!IsExecutableAddress(lpSRC) || !IsExecutableAddress(lpDST))
			return FALSE;
		CALL_REL call =
		{
			0xE8,
			0x00000000
		};
		JMP_REL jmp =
		{
			0xE9,
			0x00000000
		};
		JCC_REL jcc =
		{
			0x0F, 0x80,
			0x00000000
		};
		m_pTrampoline = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);//64字节足够了
		if (!m_pTrampoline)
			return FALSE;
		UINT8 srcPos = 0;
		UINT8 dstPos = 0;
		ULONG_PTR destJMP = 0;
		BOOL bBreak = FALSE;
		for (;;)
		{
			if (bBreak)
				break;
			HDE hs = { 0 };
			ULONG_PTR src = (ULONG_PTR)lpSRC + srcPos;
			ULONG_PTR dst = (ULONG_PTR)m_pTrampoline + dstPos;
			LPVOID pCopy = NULL;
			UINT8 copySize = HDE_DISASM((LPVOID)src, &hs);
			ASSERT(copySize == hs.len);
			if (hs.flags & F_ERROR)
				return FALSE;
			do
			{
				pCopy = (LPVOID)src;
				if (srcPos >= sizeof(JMP_REL))
				{
					jmp.operand = (UINT32)(src - (dst + sizeof(jmp)));
					pCopy = &jmp;
					copySize = sizeof(jmp);
					bBreak = TRUE;
					break;
				}
				if (hs.opcode == 0xE8)	//CALL
				{
					ULONG_PTR dest = src + hs.len + (INT32)hs.imm.imm32;
					call.operand = (UINT32)(dest - (dst + sizeof(call)));
					pCopy = &call;
					copySize = sizeof(call);
					break;
				}
				if ((hs.opcode & 0xFD) == 0xE9) //JMP
				{
					ULONG_PTR dest = src + hs.len;

					if (hs.opcode == 0xEB)
						dest += (INT8)hs.imm.imm8;
					else
						dest += (INT32)hs.imm.imm32;
					if ((ULONG_PTR)m_lpSRC <= dest
						&& dest < ((ULONG_PTR)m_lpSRC + sizeof(JMP_REL)))
					{
						if (destJMP < dest)
							destJMP = dest;
					}
					else
					{
						jmp.operand = (UINT32)(dest - (dst + sizeof(jmp)));
						pCopy = &jmp;
						copySize = sizeof(jmp);
						bBreak = (src >= destJMP);
					}
					break;
				}
				if (((hs.opcode & 0xF0) == 0x70) || (hs.opcode == 0xE3) || ((hs.opcode2 & 0xF0) == 0x80)) //JCC
				{
					ULONG_PTR dest = dst + hs.len;

					if ((hs.opcode & 0xF0) == 0x70      //JCC
						|| (hs.opcode & 0xFC) == 0xE0)  //LOOPNZ/LOOPZ/LOOP/JECXZ
						dest += (INT8)hs.imm.imm8;
					else
						dest += (INT32)hs.imm.imm32;
					if ((ULONG_PTR)m_lpSRC <= dest
						&& dest < ((ULONG_PTR)m_lpSRC + sizeof(JMP_REL)))
					{
						if (destJMP < dest)
							destJMP = dest;
					}
					else if ((hs.opcode & 0xFC) == 0xE0)
					{
						// LOOPNZ/LOOPZ/LOOP/JCXZ/JECXZ 不支持
						return FALSE;
					}
					else
					{
						UINT8 cond = ((hs.opcode != 0x0F ? hs.opcode : hs.opcode2) & 0x0F);
						jcc.opcode1 = 0x80 | cond;
						jcc.operand = (UINT32)(dest - (dst + sizeof(jcc)));
						pCopy = &jcc;
						copySize = sizeof(jcc);
					}
					break;
				}
				if (((hs.opcode & 0xFE) == 0xC2) || // ret
					((hs.opcode & 0xFD) == 0xE9) || // jmp rel
					(((hs.modrm & 0xC7) == 0x05) && ((hs.opcode == 0xFF) && (hs.modrm_reg == 4))) || // jmp rip
					((hs.opcode == 0xFF) && (hs.opcode2 == 0x25))) // jmp abs
				{
					bBreak = (src >= destJMP);
					break;
				}
			} while (0);
			if (dst < destJMP && copySize != hs.len)
				return FALSE;
			if ((dstPos + copySize) > TRAMPOLINE_MAX_SIZE)
				return FALSE;
			__movsb((LPBYTE)m_pTrampoline + dstPos, (LPBYTE)pCopy, copySize);
			srcPos += hs.len;
			dstPos += copySize;
		}
		memcpy(m_backup, lpSRC, sizeof(JMP_REL));
		m_lpSRC = lpSRC;
		m_lpDST = lpDST;
		return TRUE;
	}

	BOOL TinyInlineHook::BeginDetour()
	{
		SIZE_T size = sizeof(JMP_REL);
		LPBYTE pSRC = (LPBYTE)m_lpSRC;
		DWORD dwOldProtect = 0;
		if (!VirtualProtect(pSRC, size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			return FALSE;
		JMP_REL* pJMP = (JMP_REL*)pSRC;
		pJMP->opcode = 0xE9;
		pJMP->operand = (UINT32)((LPBYTE)m_lpDST - (pSRC + sizeof(JMP_REL)));
		if (!VirtualProtect(pSRC, size, dwOldProtect, &dwOldProtect))
			return FALSE;
		if (!FlushInstructionCache(GetCurrentProcess(), pSRC, size))
			return FALSE;
		m_bDetour = TRUE;
		return TRUE;
	}
	BOOL TinyInlineHook::EndDetour()
	{
		SIZE_T size = sizeof(JMP_REL);
		LPBYTE pSRC = (LPBYTE)m_lpSRC;
		DWORD dwOldProtect = 0;
		if (!VirtualProtect(pSRC, size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			return FALSE;
		memcpy(pSRC, m_backup, sizeof(JMP_REL));
		if (!VirtualProtect(pSRC, size, dwOldProtect, &dwOldProtect))
			return FALSE;
		if (!FlushInstructionCache(GetCurrentProcess(), pSRC, size))
			return FALSE;
		m_bDetour = FALSE;
		return TRUE;
	}
	BOOL TinyInlineHook::IsValid() const
	{
		return m_bDetour;
	}
	LPVOID TinyInlineHook::GetOrig() const
	{
		return m_pTrampoline;
	}
}