// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "Common/TinyModule.h"
#include "Control/TinyFrameUI.h"
#include "Windowless/TinyVisualHWND.h"
#include "Database/TinyAdo.h"
#include "Network/TinyConnector.h"
#include "Common/TinyLogging.h"
#include "Common/TinyAPIHook.h"
#include <algorithm>
#include "DirWatcher.h"

#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

//LogException logMsg(__FILE__, __LINE__);
//LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
//{
//	return NULL;
//}
//
//BOOL PreventSetUnhandledExceptionFilter()
//{
//	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
//	if (hKernel32 == NULL)
//		return FALSE;
//	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
//	if (pOrgEntry == NULL)
//		return FALSE;
//	unsigned char newJump[100];
//	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
//	dwOrgEntryAddr += 5;
//	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
//	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
//	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
//	newJump[0] = 0xE9;
//	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
//	SIZE_T bytesWritten;
//	BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
//	return bRet;
//}
//
//
//LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pException)
//{
//	logMsg.WriteLog(pException);
//	FatalAppExit(-1, _T("Dump OK"));
//	return EXCEPTION_CONTINUE_SEARCH;
//}
//
//void MemoryAccessCrash()
//{
//	std::cout << "Normal null pointer crash" << std::endl;
//
//	char *p = 0;
//	*p = 5;
//}
//
//void OutOfBoundsVectorCrash()
//{
//	std::cout << "std::vector out of bounds crash!" << std::endl;
//
//	std::vector<int> v;
//	v[0] = 5;
//}
//
//void AbortCrash()
//{
//	std::cout << "Calling Abort" << std::endl;
//	abort();
//}
//
//void VirtualFunctionCallCrash()
//{
//	struct B
//	{
//		B()
//		{
//			std::cout << "Pure Virtual Function Call crash!" << std::endl;
//			Bar();
//		}
//
//		virtual void Foo() = 0;
//
//		void Bar()
//		{
//			Foo();
//		}
//	};
//
//	struct D : public B
//	{
//		D()
//		{
//			std::cout << "D!" << std::endl;
//		}
//		void Foo()
//		{
//			INT a = 0;
//		}
//	};
//
//	B* b = new D;
//	b->Foo();
//}


INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	//SetUnhandledExceptionFilter(TopLevelExceptionFilter);
	//PreventSetUnhandledExceptionFilter();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	HRESULT hRes = OleInitialize(NULL);
	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	Windowless::TinyVisualHWND uiImpl;
	uiImpl.Create(NULL, 50, 50, 400, 500);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();
	OleUninitialize();
	WSACleanup();
	return loopRes;
};