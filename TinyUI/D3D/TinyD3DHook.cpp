#include "../stdafx.h"
#include "TinyD3DHook.h"

namespace TinyUI
{
	namespace D3D
	{
		D3DAPIHook::D3DAPIHook()
			:m_bHook(FALSE)
		{

		}
		BOOL D3DAPIHook::Initialize(FARPROC pfnOrig, FARPROC pfnHook)
		{
			if (!pfnOrig || !pfnHook)
				return FALSE;
			if (!VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &m_dwOrigProtect))
				return FALSE;
			memcpy(m_data, (const void*)m_pfnOrig, 5);
			return TRUE;
		}
		BOOL D3DAPIHook::BeginHook()
		{
			if (!m_pfnOrig || !m_pfnHook)
				return FALSE;
			ULONG start = ULONG(m_pfnOrig);
			ULONG target = ULONG(m_pfnHook);
			ULONG64 offset = 0;
			//http://www.cnblogs.com/zhangdongsheng/archive/2012/12/06/2804234.html
			//计算偏移量(JMP的地址–代码地址–5 = 机器码跳转地址 x86)
			offset = target - (start + 5);
			DWORD oldProtect;
			VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
			LPBYTE ps = (LPBYTE)m_pfnOrig;
			*ps = 0xE9;
			*(DWORD*)(ps + 1) = DWORD(offset);
			m_bHook = TRUE;
			return TRUE;
		}
		BOOL D3DAPIHook::EndHook()
		{
			if (!m_bHook) return FALSE;
			DWORD oldProtect;
			VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
			memcpy((void*)m_pfnOrig, m_data, 5);
			VirtualProtect((LPVOID)m_pfnOrig, 5, m_dwOrigProtect, &oldProtect);
			m_bHook = FALSE;
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		typedef HANDLE(WINAPI *CREATEREMOTETHREAD)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
		typedef BOOL(WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
		typedef LPVOID(WINAPI *VIRTUALALLOCEX)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
		typedef BOOL(WINAPI *VIRTUALFREEEX)(HANDLE, LPVOID, SIZE_T, DWORD);
		typedef HANDLE(WINAPI *OPPROC) (DWORD, BOOL, DWORD);
		BOOL InjectLibrary(HANDLE hProcess, const CHAR *pszDLL, DWORD dwSize)
		{
			if (!hProcess || !pszDLL)
				return FALSE;
			HMODULE hInstance = NULL;
			HANDLE	hThread = NULL;
			FARPROC	lpStartAddress = NULL;
			dwSize = (dwSize + 1) * sizeof(CHAR);
			WRITEPROCESSMEMORY pfnWriteProcessMemory = NULL;
			CREATEREMOTETHREAD pfnCreateRemoteThread = NULL;
			VIRTUALALLOCEX pfnVirtualAllocEx = NULL;
			VIRTUALFREEEX pfnVirtualFreeEx = NULL;
			INT obfSize = 12;
			CHAR pWPMSTR[19], pCRTSTR[19], pVAESTR[15], pVFESTR[14], pLLSTR[13];
			memcpy(pWPMSTR, "RvnrdPqmni|}Dmfegm", 19);
			memcpy(pCRTSTR, "FvbgueQg`c{k]`yotp", 19);
			memcpy(pVAESTR, "WiqvpekGeddiHt", 15);
			memcpy(pVFESTR, "Wiqvpek@{mnOu", 14);
			memcpy(pLLSTR, "MobfImethzr", 12);
			pLLSTR[11] = 'A';
			pLLSTR[12] = 0;
			obfSize += 6;
			for (INT i = 0; i < obfSize; i++) pWPMSTR[i] ^= i ^ 5;
			for (INT i = 0; i < obfSize; i++) pCRTSTR[i] ^= i ^ 5;
			obfSize -= 4;
			for (INT i = 0; i < obfSize; i++) pVAESTR[i] ^= i ^ 1;
			obfSize -= 1;
			for (INT i = 0; i < obfSize; i++) pVFESTR[i] ^= i ^ 1;
			obfSize -= 2;
			for (INT i = 0; i < obfSize; i++) pLLSTR[i] ^= i ^ 1;
			hInstance = GetModuleHandle(TEXT("KERNEL32"));
			if (!hInstance)
				return FALSE;

			pfnWriteProcessMemory = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, pWPMSTR);
			pfnCreateRemoteThread = (CREATEREMOTETHREAD)GetProcAddress(hInstance, pCRTSTR);
			pfnVirtualAllocEx = (VIRTUALALLOCEX)GetProcAddress(hInstance, pVAESTR);
			pfnVirtualFreeEx = (VIRTUALFREEEX)GetProcAddress(hInstance, pVFESTR);
			if (!pfnWriteProcessMemory || !pfnCreateRemoteThread || !pfnVirtualAllocEx || !pfnVirtualFreeEx)
				return FALSE;

			LPVOID pAlloc = (LPVOID)(*pfnVirtualAllocEx)(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!pAlloc)
				return FALSE;

			SIZE_T  writtenSize = 0;
			BOOL bRes = (*pfnWriteProcessMemory)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &writtenSize);
			if (!bRes)
				goto error;
			lpStartAddress = (FARPROC)GetProcAddress(hInstance, pLLSTR);
			if (!lpStartAddress)
			{
				bRes = FALSE;
				goto error;
			}
			hThread = (*pfnCreateRemoteThread)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(lpStartAddress), pAlloc, 0, 0);
			if (!hThread)
			{
				bRes = FALSE;
				goto error;
			}
			if (WaitForSingleObject(hThread, 200) == WAIT_OBJECT_0)
			{
				DWORD dw;
				GetExitCodeThread(hThread, &dw);
				bRes = dw != 0;
			}
		error:
			if (hThread)
			{
				CloseHandle(hThread);
				hThread = NULL;
			}
			if (pAlloc)
			{
				(*pfnVirtualFreeEx)(hProcess, pAlloc, 0, MEM_RELEASE);
				pAlloc = NULL;
			}
			return bRes;
		}
	}
}
