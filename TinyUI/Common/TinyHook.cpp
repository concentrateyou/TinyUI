#include "../stdafx.h"
#include "TinyModule.h"
#include "TinyString.h"
#include "TinyHook.h"
#include "TinyLogging.h"
#include "TinyApplication.h"

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
	static BOOL IsCodePadding(LPBYTE pInst, UINT size)
	{
		UINT i;

		if (pInst[0] != 0x00 && pInst[0] != 0x90 && pInst[0] != 0xCC)
			return FALSE;

		for (i = 1; i < size; ++i)
		{
			if (pInst[i] != pInst[0])
				return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
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
		:m_lpSRC(NULL),
		m_lpDST(NULL),
		m_lpRelay(NULL),
		m_bDetour(FALSE),
		m_bAbove(FALSE)
	{

	}
	TinyDetour::~TinyDetour()
	{
		Uninitialize();
	}
	BOOL TinyDetour::Uninitialize()
	{
		BOOL bRes = TRUE;
		if (m_pTrampoline)
		{
			bRes &= VirtualFree(m_pTrampoline, 0, MEM_RELEASE);
			m_pTrampoline = NULL;
		}
		return bRes;
	}
	BOOL TinyDetour::Initialize(LPVOID lpSRC, LPVOID lpDST)
	{
		if (!IsExecutableAddress(lpSRC) || !IsExecutableAddress(lpDST))
		{
			LOG(INFO) << "!IsExecutableAddress: || !IsExecutableAddress FAIL\n";
			return FALSE;
		}
#if defined(_WIN64)
		CALL_ABS call =
		{
			0xFF, 0x15, 0x00000002,
			0xEB, 0x08,
			0x0000000000000000ULL
		};
		JMP_ABS jmp =
		{
			0xFF, 0x25, 0x00000000,
			0x0000000000000000ULL
		};
		JCC_ABS jcc =
		{
			0x70, 0x0E,
			0xFF, 0x25, 0x00000000,
			0x0000000000000000ULL
		};
#else
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
#endif
		m_pTrampoline = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);//64字节足够了
		if (!m_pTrampoline)
		{
			LOG(INFO) << "VirtualAlloc FAIL\n";
			return FALSE;
		}
		LOG(INFO) << "VirtualAlloc OK\n";
		UINT8 srcPos = 0;
		UINT8 dstPos = 0;
		ULONG_PTR destJMP = 0;
		BOOL bBreak = FALSE;
		for (;;)
		{
			if (bBreak)
			{
				LOG(INFO) << "bBreak = TRUE\n";
				break;
			}
			LOG(INFO) << "bBreak = FALSE\n";
			HDE hs = { 0 };
			ULONG_PTR src = (ULONG_PTR)lpSRC + srcPos;
			ULONG_PTR dst = (ULONG_PTR)m_pTrampoline + dstPos;
			LPVOID pCopy = NULL;
			LOG(INFO) << "Begin HDE_DISASM\n";
			UINT8 copySize = HDE_DISASM((LPVOID)src, &hs);
			LOG(INFO) << "End HDE_DISASM\n";
			ASSERT(copySize == hs.len);
			if (hs.flags & F_ERROR)
			{
				LOG(INFO) << "hs.flags & F_ERROR\n";
				return FALSE;
			}
			do
			{
				pCopy = (LPVOID)src;
				if (srcPos >= sizeof(JMP_REL))
				{
#if defined(_WIN64)
					jmp.address = src;
#else
					jmp.operand = (UINT32)(src - (dst + sizeof(jmp)));
#endif
					pCopy = &jmp;
					copySize = sizeof(jmp);
					bBreak = TRUE;
					LOG(INFO) << "srcPos >= sizeof(JMP_REL)\n";
					break;
				}
				if (hs.opcode == 0xE8)	//CALL
				{
					ULONG_PTR dest = src + hs.len + (INT32)hs.imm.imm32;
#if defined(_WIN64)
					call.address = dest;
#else
					call.operand = (UINT32)(dest - (dst + sizeof(call)));
#endif 
					pCopy = &call;
					copySize = sizeof(call);
					LOG(INFO) << "hs.opcode == 0xE8\n";
					break;
				}
				if ((hs.opcode & 0xFD) == 0xE9) //JMP
				{
					ULONG_PTR dest = src + hs.len;

					if (hs.opcode == 0xEB)
						dest += (INT8)hs.imm.imm8;
					else
						dest += (INT32)hs.imm.imm32;
					if ((ULONG_PTR)m_lpSRC <= dest && dest < ((ULONG_PTR)m_lpSRC + sizeof(JMP_REL)))
					{
						if (destJMP < dest)
							destJMP = dest;
					}
					else
					{
#if defined(_WIN64)
						jmp.address = dest;
#else
						jmp.operand = (UINT32)(dest - (dst + sizeof(jmp)));
#endif // _WIN64
						pCopy = &jmp;
						copySize = sizeof(jmp);
						bBreak = (src >= destJMP);
					}
					LOG(INFO) << "(hs.opcode & 0xFD) == 0xE9n";
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
						LOG(INFO) << "LOOPNZ/LOOPZ/LOOP/JCXZ/JECXZ 不支持\n";
						return FALSE;
					}
					else
					{
						UINT8 cond = ((hs.opcode != 0x0F ? hs.opcode : hs.opcode2) & 0x0F);
#if defined(_WIN64)
						jcc.opcode = 0x71 ^ cond;
						jcc.address = dest;
#else
						jcc.opcode1 = 0x80 | cond;
						jcc.operand = (UINT32)(dest - (dst + sizeof(jcc)));
#endif
						pCopy = &jcc;
						copySize = sizeof(jcc);
					}
					LOG(INFO) << "((hs.opcode & 0xF0) == 0x70) || (hs.opcode == 0xE3) || ((hs.opcode2 & 0xF0) == 0x80)\n";
					break;
				}
				if (((hs.opcode & 0xFE) == 0xC2) || // RET
					((hs.opcode & 0xFD) == 0xE9) || // JMP REL
					(((hs.modrm & 0xC7) == 0x05) && ((hs.opcode == 0xFF) && (hs.modrm_reg == 4))) || // JMP RIP
					((hs.opcode == 0xFF) && (hs.opcode2 == 0x25))) // JMP ABS
				{
					bBreak = (src >= destJMP);
					LOG(INFO) << "bBreak = (src >= destJMP)\n";
					break;
				}
			} while (0);
			LOG(INFO) << "test-----1\n";
			if (dst < destJMP && copySize != hs.len)
			{
				LOG(INFO) << "dst < destJMP && copySize != hs.len\n";
				return FALSE;
			}
			if ((dstPos + copySize) > TRAMPOLINE_MAX_SIZE)
			{
				LOG(INFO) << "(dstPos + copySize) > TRAMPOLINE_MAX_SIZE\n";
				return FALSE;
			}
			LOG(INFO) << "__movsb\n";
			__movsb((LPBYTE)m_pTrampoline + dstPos, (LPBYTE)pCopy, copySize);
			srcPos += hs.len;
			dstPos += copySize;
		}
		//如果偏移量小于5字节 Try 短跳
		if (srcPos < sizeof(JMP_REL) && !IsCodePadding((LPBYTE)m_lpSRC + srcPos, sizeof(JMP_REL) - srcPos))
		{
			LOG(INFO) << "srcPos < sizeof(JMP_REL) && !IsCodePadding((LPBYTE)m_lpSRC + srcPos, sizeof(JMP_REL) - srcPos)\n";
			//判断是否支持短跳
			if (srcPos < sizeof(JMP_REL_SHORT) && !IsCodePadding((LPBYTE)m_lpSRC + srcPos, sizeof(JMP_REL_SHORT) - srcPos))
			{
				LOG(INFO) << "srcPos < sizeof(JMP_REL_SHORT) && !IsCodePadding((LPBYTE)m_lpSRC + srcPos, sizeof(JMP_REL_SHORT) - srcPos)\n";
				return FALSE;
			}
			if (!IsExecutableAddress((LPBYTE)m_lpSRC - sizeof(JMP_REL)))
			{
				LOG(INFO) << "!IsExecutableAddress((LPBYTE)m_lpSRC - sizeof(JMP_REL))\n";
				return FALSE;
			}
			if (!IsCodePadding((LPBYTE)m_lpSRC - sizeof(JMP_REL), sizeof(JMP_REL)))
			{
				LOG(INFO) << "!IsCodePadding((LPBYTE)m_lpSRC - sizeof(JMP_REL), sizeof(JMP_REL))\n";
				return FALSE;
			}
			memcpy(m_backup, (LPBYTE)m_lpSRC - sizeof(JMP_REL), sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
		}
		else
		{
			LOG(INFO) << "memcpy(m_backup, lpSRC, sizeof(JMP_REL))\n";
			memcpy(m_backup, lpSRC, sizeof(JMP_REL));
		}
		m_lpSRC = lpSRC;
		m_lpDST = lpDST;

#if defined(_WIN64)
		jmp.address = (ULONG_PTR)m_lpDST;
		m_lpRelay = (LPBYTE)m_pTrampoline + dstPos;
		memcpy(m_lpRelay, &jmp, sizeof(jmp));
#endif
		LOG(INFO) << "TinyDetour Initialize OK\n";
		return TRUE;
	}

	BOOL TinyDetour::BeginDetour()
	{
		SIZE_T size = sizeof(JMP_REL);
		LPBYTE pSRC = (LPBYTE)m_lpSRC;
#if defined(_WIN64)
		LPBYTE pDST = (LPBYTE)m_lpRelay;
#else
		LPBYTE pDST = (LPBYTE)m_lpDST;
#endif
		if (m_bAbove)
		{
			pSRC -= sizeof(JMP_REL);
			size += sizeof(JMP_REL_SHORT);
		}
		DWORD dwOldProtect = 0;
		if (!VirtualProtect(pSRC, size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			return FALSE;
		JMP_REL* pJMP = (JMP_REL*)pSRC;
		pJMP->opcode = 0xE9;
		pJMP->operand = (UINT32)((LPBYTE)pDST - (pSRC + sizeof(JMP_REL)));

		if (m_bAbove)
		{
			PJMP_REL_SHORT pShortJMP = (PJMP_REL_SHORT)m_lpSRC;
			pShortJMP->opcode = 0xEB;
			pShortJMP->operand = (UINT8)(0 - (sizeof(JMP_REL_SHORT) + sizeof(JMP_REL)));
		}
		if (!VirtualProtect(pSRC, size, dwOldProtect, &dwOldProtect))
			return FALSE;
		if (!FlushInstructionCache(GetCurrentProcess(), pSRC, size))
			return FALSE;
		m_bDetour = TRUE;
		return TRUE;
	}
	BOOL TinyDetour::EndDetour()
	{
		SIZE_T size = sizeof(JMP_REL);
		LPBYTE pSRC = (LPBYTE)m_lpSRC;
		DWORD dwOldProtect = 0;
		if (!VirtualProtect(pSRC, size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			return FALSE;
		if (m_bAbove)
		{
			memcpy(pSRC, m_backup, sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
		}
		else
		{
			memcpy(pSRC, m_backup, sizeof(JMP_REL));
		}
		if (!VirtualProtect(pSRC, size, dwOldProtect, &dwOldProtect))
			return FALSE;
		if (!FlushInstructionCache(GetCurrentProcess(), pSRC, size))
			return FALSE;
		m_bDetour = FALSE;
		return TRUE;
	}
	BOOL TinyDetour::IsValid() const
	{
		return m_bDetour;
	}
	LPVOID TinyDetour::GetOrig() const
	{
		return m_pTrampoline;
	}
}