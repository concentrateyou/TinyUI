// DXApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "DXApp.h"
#include "DXApplication.h"
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")
using namespace DXFramework;

namespace DXApp
{
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
}

INT APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DXApp::LoadSeDebugPrivilege();

	BOOL bComposition = FALSE;
	DwmIsCompositionEnabled(&bComposition);

	DwmEnableComposition(FALSE);
	DXApp::DXApplication app;
	app.Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_DXAPP));

	INT iRes = app.Run();
	DwmEnableComposition(TRUE);
	return iRes;
}

