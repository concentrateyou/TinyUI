#include "stdafx.h"
#include "D3DHook.h"

namespace D3D
{
	typedef HANDLE(WINAPI *CREATEREMOTETHREAD)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
	typedef BOOL(WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
	typedef LPVOID(WINAPI *VIRTUALALLOCEX)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
	typedef BOOL(WINAPI *VIRTUALFREEEX)(HANDLE, LPVOID, SIZE_T, DWORD);
	typedef HANDLE(WINAPI *LOADLIBRARY) (DWORD, BOOL, DWORD);
	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
	{
		if (!hProcess || !pszDLL)
			return FALSE;
		HMODULE hInstance = NULL;
		HANDLE	hThread = NULL;
		FARPROC	lpStartAddress = NULL;
		DWORD dwSize = strlen(pszDLL);
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
		SIZE_T size = 0;
		BOOL bRes = (*pfnWriteProcessMemory)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
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