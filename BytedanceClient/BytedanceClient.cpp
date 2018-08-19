// BytedanceClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "BytedanceClient.h"
#include "Common/TinyApplication.h"
#include "MainView.h"
using namespace TinyFramework;
using namespace Bytedance;

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

//class ClassA
//{
//public:
//	virtual ~ClassA() {};
//	virtual void FunctionA1() {};
//	void FonctionA2() {};
//};
//class ClassB
//{
//public:
//	virtual void FunctionB1() {};
//	void FonctionB2() {};
//};
//class ClassC : public ClassA, public ClassB
//{
//public:
//	void FunctionA1() {};
//	void FonctionA2() {};
//	void FunctionB1() {};
//	void FonctionB2() {};
//};

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	/*ClassC aObject;
	ClassA* pA = &aObject;
	ClassB* pB = &aObject;
	ClassC* pC = &aObject;*/

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hRes = OleInitialize(NULL);
	LoadSeDebugPrivilege();
	CoInitialize(NULL);

	TinyVisualResource::GetInstance().Load("skin\\resource.xml");

	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_BYTEDANCECLIENT));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	MainView window;
	window.Create(NULL, "");
	window.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	CoUninitialize();
	OleUninitialize();
	WSACleanup();

	return loopRes;

};