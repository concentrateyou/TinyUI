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
#include <algorithm>
#include "DirWatcher.h"

#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

LogException logMsg(__FILE__, __LINE__);

LONG NTAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS pExcepInfo)
{
	logMsg.WriteLog(pExcepInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG NTAPI FirstVectoredExceptionFilter(PEXCEPTION_POINTERS pExcepInfo)
{
	logMsg.WriteLog(pExcepInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG NTAPI LastVectoredExceptionFilter(PEXCEPTION_POINTERS pExcepInfo)
{
	logMsg.WriteLog(pExcepInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void Test(MINIDUMP_EXCEPTION_INFORMATION* ps)
{
	DWORD id = ps->ThreadId;
}

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	RemoveVectoredExceptionHandler(&FirstVectoredExceptionFilter);
	RemoveVectoredContinueHandler(&LastVectoredExceptionFilter);

	SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
	AddVectoredExceptionHandler(0, &FirstVectoredExceptionFilter);
	AddVectoredContinueHandler(0, &LastVectoredExceptionFilter);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Test(NULL);

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