// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "FLVPlayer.h"
#include "FLVFrameUI.h"
#include "FFPlayer.h"
#include "FLVParser.h"
#include "QSVView.h"

using namespace FLVPlayer;

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

class A
{
public:
	A()
	{
		TRACE("A");
	}
	~A()
	{
		TRACE("~A");
	}
};

class B
{
public:
	B()
	{
		TRACE("B");
	}
	~B()
	{
		TRACE("~B");
	}
};


class Application
{
public:
	Application()
	{

	}
	BOOL Initialize()
	{
		return TRUE;
	}
	static Application& GetInstance()
	{
		static Application b;
		return b;
	}

	~Application()
	{
		TRACE("~Application");
	}

private:
	A a;
	B b;
};

class MShowApp
{
public:
	MShowApp()
	{

	}

	static MShowApp& GetInstance()
	{
		static MShowApp c;
		return c;
	}

	BOOL Initialize()
	{
		
		return TRUE;
	}

	~MShowApp()
	{
		TRACE("~MShowApp");
	}
};

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hRes = OleInitialize(NULL);

	LoadSeDebugPrivilege();

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_FLVPLAYER));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	FLVFrameUI uiImpl;
	uiImpl.Create(NULL, 50, 50, 800, 600);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	OleUninitialize();
	WSACleanup();


	MShowApp& app = MShowApp::GetInstance();
	app.Initialize();

	Application& app1 = Application::GetInstance();
	app1.Initialize();

	return loopRes;
};