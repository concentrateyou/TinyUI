#include "stdafx.h"
#include "DXFramework.h"
#include "Control/TinyControl.h"

namespace DXFramework
{
	typedef HANDLE(WINAPI *CREATEREMOTETHREAD)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
	typedef BOOL(WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
	typedef LPVOID(WINAPI *VIRTUALALLOCEX)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
	typedef BOOL(WINAPI *VIRTUALFREEEX)(HANDLE, LPVOID, SIZE_T, DWORD);
	typedef HANDLE(WINAPI *LOADLIBRARY) (DWORD, BOOL, DWORD);
	typedef HANDLE(WINAPI *FREELIBRARY) (HMODULE);

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
	{
		if (!hProcess || !pszDLL)
			return FALSE;
		HMODULE hInstance = NULL;
		HANDLE	hTask = NULL;
		FARPROC	address = NULL;
		WRITEPROCESSMEMORY pfnWriteProcessMemory = NULL;
		CREATEREMOTETHREAD pfnCreateRemoteThread = NULL;
		VIRTUALALLOCEX pfnVirtualAllocEx = NULL;
		VIRTUALFREEEX pfnVirtualFreeEx = NULL;
		hInstance = GetModuleHandle(TEXT("KERNEL32"));
		if (!hInstance)
			return FALSE;
		pfnWriteProcessMemory = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
		pfnCreateRemoteThread = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
		pfnVirtualAllocEx = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
		pfnVirtualFreeEx = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
		if (!pfnWriteProcessMemory || !pfnCreateRemoteThread || !pfnVirtualAllocEx || !pfnVirtualFreeEx)
			return FALSE;
		DWORD dwSize = strlen(pszDLL);
		dwSize = (dwSize + 1) * sizeof(CHAR);
		LPVOID pAlloc = (LPVOID)(*pfnVirtualAllocEx)(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!pAlloc)
			return FALSE;
		SIZE_T size = 0;
		BOOL bRes = (*pfnWriteProcessMemory)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
		if (!bRes)
			goto error;
		address = (FARPROC)GetProcAddress(hInstance, TEXT("LoadLibraryA"));
		if (!address)
		{
			bRes = FALSE;
			goto error;
		}
		hTask = (*pfnCreateRemoteThread)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), pAlloc, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto error;
		}
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetExitCodeThread(hTask, &dw);
			bRes = dw != 0;
		}
	error:
		if (hTask != NULL)
		{
			CloseHandle(hTask);
			hTask = NULL;
		}
		if (pAlloc != NULL)
		{
			(*pfnVirtualFreeEx)(hProcess, pAlloc, 0, MEM_RELEASE);
			pAlloc = NULL;
		}
		return bRes;
	}
	BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
	{
		if (!hProcess || !pszDLL)
			return FALSE;
		HMODULE hInstance = NULL;
		HANDLE	hTask = NULL;
		FARPROC	address = NULL;
		WRITEPROCESSMEMORY pfnWriteProcessMemory = NULL;
		CREATEREMOTETHREAD pfnCreateRemoteThread = NULL;
		VIRTUALALLOCEX pfnVirtualAllocEx = NULL;
		VIRTUALFREEEX pfnVirtualFreeEx = NULL;
		hInstance = GetModuleHandle(TEXT("KERNEL32"));
		if (!hInstance)
			return FALSE;
		pfnWriteProcessMemory = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
		pfnCreateRemoteThread = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
		pfnVirtualAllocEx = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
		pfnVirtualFreeEx = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
		if (!pfnWriteProcessMemory || !pfnCreateRemoteThread || !pfnVirtualAllocEx || !pfnVirtualFreeEx)
			return FALSE;
		DWORD dwSize = strlen(pszDLL);
		dwSize = (dwSize + 1) * sizeof(CHAR);
		LPVOID pAlloc = (LPVOID)(*pfnVirtualAllocEx)(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!pAlloc)
			return FALSE;
		SIZE_T size = 0;
		BOOL bRes = (*pfnWriteProcessMemory)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
		if (!bRes)
			goto error;
		address = (FARPROC)GetProcAddress(hInstance, TEXT("GetModuleHandleA"));
		if (!address)
		{
			bRes = FALSE;
			goto error;
		}
		hTask = (*pfnCreateRemoteThread)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), pAlloc, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto error;
		}
		DWORD dwHandle = 0;
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(hTask, &dwHandle);
			if (dwHandle == 0)
			{
				bRes = FALSE;
				goto error;
			}
		}
		address = (FARPROC)GetProcAddress(hInstance, TEXT("FreeLibrary"));
		if (!address)
		{
			bRes = FALSE;
			goto error;
		}
		hTask = (*pfnCreateRemoteThread)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), (LPVOID)dwHandle, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto error;
		}
		if (WaitForSingleObject(hTask, 5000) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetExitCodeThread(hTask, &dw);
			bRes = dw != 0;
		}
	error:
		if (hTask != NULL)
		{
			CloseHandle(hTask);
			hTask = NULL;
		}
		if (pAlloc != NULL)
		{
			(*pfnVirtualFreeEx)(hProcess, pAlloc, 0, MEM_RELEASE);
			pAlloc = NULL;
		}
		TRACE("UninjectLibrary\n");
		return bRes;
	}
}