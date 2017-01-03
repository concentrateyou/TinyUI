#include "stdafx.h"
#include "DXApplication.h"

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
DXApplication::DXApplication()
{
}
DXApplication::~DXApplication()
{
}
BOOL DXApplication::Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName)
{
	LoadSeDebugPrivilege();
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	::DefWindowProc(NULL, 0, 0, 0L);
	if (!TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, lpTableName))
		return FALSE;
	if (!TinyApplication::GetInstance()->AddMessageLoop(&m_msgLoop))
		return FALSE;
	m_frameUI.Reset(new DXFrameUI());
	return m_frameUI->Create(NULL, 0, 0, 1024, 720);
}
INT DXApplication::Run()
{
	INT loopRes = m_msgLoop.MessageLoop();
	this->Uninitialize();
	return loopRes;
}
BOOL DXApplication::Uninitialize()
{
	if (!TinyApplication::GetInstance()->RemoveMessageLoop())
		return FALSE;
	if (!TinyApplication::GetInstance()->Uninitialize())
		return FALSE;
	return TRUE;
}
DXFrameUI* DXApplication::GetFrameUI()
{
	return m_frameUI.Ptr();
}