// MShowApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MShowApp.h"
#include "RTMPReader.h"

using namespace std;

namespace MShow
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
	//////////////////////////////////////////////////////////////////////////
	MShowApp::MShowApp()
	{
	}
	MShowApp::~MShowApp()
	{
	}
	BOOL MShowApp::Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName)
	{
		LoadSeDebugPrivilege();
		UNREFERENCED_PARAMETER(lpCmdLine);
		UNREFERENCED_PARAMETER(nCmdShow);
		::DefWindowProc(NULL, 0, 0, 0L);
		if (!TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, lpTableName))
			return FALSE;
		if (!TinyApplication::GetInstance()->AddMessageLoop(&m_msgLoop))
			return FALSE;
		return m_mshow.Create(NULL, 0, 0, 1, 1);
	}
	INT MShowApp::Run()
	{
		INT loopRes = m_msgLoop.MessageLoop();
		this->Uninitialize();
		return loopRes;
	}
	BOOL MShowApp::Uninitialize()
	{
		if (!TinyApplication::GetInstance()->RemoveMessageLoop())
			return FALSE;
		if (!TinyApplication::GetInstance()->Uninitialize())
			return FALSE;
		return TRUE;
	}
	MShowWindow* MShowApp::GetWindow()
	{
		return &m_mshow;
	}
}

//////////////////////////////////////////////////////////////////////////
INT APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MShow::MShowApp app;
	app.Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_MSHOWAPP));
	INT iRes = app.Run();
	return iRes;
}
