// InjectHelper.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Common/TinyCommon.h"
#include "IO/TinyProcess.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;

BOOL LoadSeDebugPrivilege()
{
	DWORD   err;
	HANDLE  hToken;
	LUID    Val;
	TOKEN_PRIVILEGES tp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		err = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}
typedef HANDLE(WINAPI *CREATEREMOTETHREAD)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
typedef LPVOID(WINAPI *VIRTUALALLOCEX)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
typedef BOOL(WINAPI *VIRTUALFREEEX)(HANDLE, LPVOID, SIZE_T, DWORD);
typedef HANDLE(WINAPI *LOADLIBRARY) (DWORD, BOOL, DWORD);
typedef HANDLE(WINAPI *FREELIBRARY) (HMODULE);
typedef HHOOK(WINAPI *SETWINDOWSHOOKEX)(INT, HOOKPROC, HINSTANCE, DWORD);
#define RETRY_INTERVAL_MS      500
#define TOTAL_RETRY_TIME_MS    4000
#define RETRY_COUNT            (TOTAL_RETRY_TIME_MS / RETRY_INTERVAL_MS)

static BOOL InjectLibrary(HANDLE hProcess, const WCHAR* dll)
{
	if (!hProcess || !PathFileExistsW(dll))
		return FALSE;
	HRESULT hRes = E_FAIL;
	HMODULE hInstance = NULL;
	HANDLE	hTask = NULL;
	FARPROC	address = NULL;
	hInstance = GetModuleHandleW(L"KERNEL32");
	if (!hInstance)
		return FALSE;
	CREATEREMOTETHREAD pCREATEREMOTETHREAD = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
	if (!pCREATEREMOTETHREAD)
		return FALSE;
	WRITEPROCESSMEMORY pWRITEPROCESSMEMORY = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
	if (!pWRITEPROCESSMEMORY)
		return FALSE;
	VIRTUALALLOCEX pVIRTUALALLOCEX = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
	if (!pVIRTUALALLOCEX)
		return FALSE;
	VIRTUALFREEEX pVIRTUALFREEEX = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
	if (!pVIRTUALFREEEX)
		return FALSE;
	address = (FARPROC)GetProcAddress(hInstance, TEXT("LoadLibraryW"));
	if (!address)
		return FALSE;
	DWORD	dwID = 0;
	LPVOID	pDATA = NULL;
	SIZE_T	wsize = 0;
	UINT32	size = (wcslen(dll) + 1) * sizeof(WCHAR);
	pDATA = pVIRTUALALLOCEX(hProcess, NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!pDATA)
		return FALSE;
	if (!pWRITEPROCESSMEMORY(hProcess, pDATA, dll, size, &wsize))
	{
		goto _ERROR;
	}
	hTask = pCREATEREMOTETHREAD(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, pDATA, 0, &dwID);
	if (!hTask)
	{
		goto _ERROR;
	}
	if (WaitForSingleObject(hTask, 4000) == WAIT_OBJECT_0)
	{
		dwID = 0;
		GetExitCodeThread(hTask, &dwID);
		SetLastError(0);
		hRes = (dwID == 0) ? S_OK : E_FAIL;
	}
_ERROR:
	if (FAILED(hRes))
	{
		dwID = GetLastError();
	}
	SAFE_DELETE_HANDLE(hTask);
	if (pDATA != NULL)
	{
		pVIRTUALFREEEX(hProcess, pDATA, 0, MEM_RELEASE);
		pDATA = NULL;
	}
	if (dwID != 0)
	{
		SetLastError(dwID);
	}
	return SUCCEEDED(hRes);
}

BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
{
	if (!hProcess || !pszDLL)
		return FALSE;
	HMODULE hInstance = NULL;
	HANDLE	hTask = NULL;
	FARPROC	address = NULL;
	hInstance = GetModuleHandle(TEXT("KERNEL32"));
	if (!hInstance)
		return FALSE;
	CREATEREMOTETHREAD pCREATEREMOTETHREAD = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
	if (!pCREATEREMOTETHREAD)
		return FALSE;
#if defined(_WIN64)
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return FALSE;
	MODULEENTRY32 me;
	ZeroMemory(&me, sizeof(MODULEENTRY32));
	me.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hSnapshot, &me))
		return FALSE;
	BOOL bRes = TRUE;
	do
	{
		if (strncasecmp(pszDLL, me.szExePath, strlen(pszDLL)) == 0)
		{
			bRes = TRUE;
			break;
		}
	} while (Module32Next(hSnapshot, &me));
	if (!bRes)
	{
		goto _ERROR;
	}
	address = (FARPROC)GetProcAddress(hInstance, TEXT("FreeLibrary"));
	if (!address)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), (LPVOID)me.modBaseAddr, 0, 0);
	if (!hTask)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
	{
		DWORD dw;
		GetExitCodeThread(hTask, &dw);
		bRes = dw != 0;
	}
_ERROR:
	if (hTask != NULL)
	{
		CloseHandle(hTask);
		hTask = NULL;
	}
	TRACE("UninjectLibrary\n");
	return bRes;
#else
	WRITEPROCESSMEMORY pWRITEPROCESSMEMORY = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
	if (!pWRITEPROCESSMEMORY)
		return FALSE;
	VIRTUALALLOCEX pVIRTUALALLOCEX = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
	if (!pVIRTUALALLOCEX)
		return FALSE;
	VIRTUALFREEEX pVIRTUALFREEEX = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
	if (!pVIRTUALFREEEX)
		return FALSE;
	DWORD dwSize = (strlen(pszDLL) + 1) * sizeof(CHAR);
	LPVOID pAlloc = (LPVOID)(*pVIRTUALALLOCEX)(hProcess, NULL, dwSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pAlloc)
		return FALSE;
	SIZE_T size = 0;
	BOOL bRes = (*pWRITEPROCESSMEMORY)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
	if (!bRes)
		goto _ERROR;
	address = (FARPROC)GetProcAddress(hInstance, TEXT("GetModuleHandleA"));
	if (!address)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), pAlloc, 0, 0);
	if (!hTask)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	DWORD dwHandle = 0;
	if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
	{
		GetExitCodeThread(hTask, &dwHandle);
		if (dwHandle == 0)
		{
			bRes = FALSE;
			goto _ERROR;
		}
	}
	address = (FARPROC)GetProcAddress(hInstance, TEXT("FreeLibrary"));
	if (!address)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), (LPVOID)dwHandle, 0, 0);
	if (!hTask)
	{
		bRes = FALSE;
		goto _ERROR;
	}
	if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
	{
		DWORD dw;
		GetExitCodeThread(hTask, &dw);
		bRes = dw != 0;
	}
_ERROR:
	if (hTask != NULL)
	{
		CloseHandle(hTask);
		hTask = NULL;
	}
	if (pAlloc != NULL)
	{
		(*pVIRTUALFREEEX)(hProcess, pAlloc, 0, MEM_RELEASE);
		pAlloc = NULL;
	}
	TRACE("UninjectLibrary\n");
	return bRes;
#endif
}

static BOOL InjectLibrarySafe(DWORD dwID, const WCHAR* dll)
{
	HHOOK	hHook = NULL;
	HMODULE hUser32 = NULL;
	HMODULE hInstance = NULL;
	LPVOID	address = NULL;
	hUser32 = GetModuleHandleW(L"USER32");
	if (!hUser32)
		return FALSE;
	hInstance = LoadLibraryW(dll);
	if (!hInstance)
		return FALSE;
	address = GetProcAddress(hInstance, "_GameDetourProc@12");
	if (!address)
	{
		goto _ERROR;
	}
	SETWINDOWSHOOKEX pSETWINDOWSHOOKEX = (SETWINDOWSHOOKEX)GetProcAddress(hUser32, TEXT("SetWindowsHookExW"));
	if (!pSETWINDOWSHOOKEX)
	{
		goto _ERROR;
	}
	hHook = pSETWINDOWSHOOKEX(WH_GETMESSAGE, (HOOKPROC)address, hInstance, dwID);
	if (!hHook)
	{
		goto _ERROR;
	}
	for (INT32 i = 0; i < RETRY_COUNT; i++)
	{
		Sleep(RETRY_INTERVAL_MS);
		PostThreadMessage(dwID, WM_USER + 432, 0, (LPARAM)hHook);
	}
	return TRUE;
_ERROR:
	if (hInstance != NULL)
	{
		FreeLibrary(hInstance);
		hInstance = NULL;
	}
	return FALSE;
}

static BOOL InjectLibraryFull(DWORD dwID, const WCHAR* dll)
{
	TinyProcess process;
	if (!process.Open(PROCESS_ALL_ACCESS, FALSE, dwID))
		return FALSE;
	return InjectLibrary(process, dll);
}

static BOOL InjectHelper(WCHAR *argv[], const WCHAR *dll)
{
	DWORD dwID = 0;
	DWORD bSAFE = FALSE;
	bSAFE = wcstol(argv[2], NULL, 10);
	dwID = wcstol(argv[3], NULL, 10);
	if (dwID == 0)
		return FALSE;
	return bSAFE ? InjectLibrarySafe(dwID, dll) : InjectLibraryFull(dwID, dll);
}

int main(int argc, char *argv_ansi[])
{
	SetErrorMode(SEM_FAILCRITICALERRORS);
	LoadSeDebugPrivilege();
	BOOL  bRes = 0;
	WCHAR dll[MAX_PATH];
	LPWSTR pCommandLineW = NULL;
	LPWSTR *argv = NULL;
	pCommandLineW = GetCommandLineW();
	argv = CommandLineToArgvW(pCommandLineW, &argc);
	if (argv != NULL)
	{
		if (argc == 4)
		{
			DWORD dwSize = 0;
			dwSize = GetModuleFileNameW(NULL, dll, MAX_PATH);
			if (dwSize > 0)
			{
				WCHAR *start = wcsrchr(dll, '\\');
				if (start != NULL)
				{
					*(++start) = 0;
					wcscpy(start, argv[1]);
					bRes = InjectHelper(argv, dll);
				}
			}
		}
		LocalFree(argv);
	}
	UNUSED(argv_ansi);
	return 0;
}

