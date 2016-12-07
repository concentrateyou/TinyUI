// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "Windowless/TinyVisualHWND.h"
#include "Windowless/TinyVisualRichText.h"
#include "Render/TinyDDraw.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Network/TinyHTTPClient.h"


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

//class A
//{
//public:
//	A(const char *pstr) {
//		TRACE("constructor\n");
//		m_data = (pstr != 0 ? strcpy(new char[strlen(pstr) + 1], pstr) : 0);
//	}
//	A(const A &a) {
//		TRACE("copy constructor\n");
//		m_data = (a.m_data != 0 ? strcpy(new char[strlen(a.m_data) + 1], a.m_data) : 0);
//	}
//	A &operator =(const A &a) {
//		TRACE("copy assigment\n");
//		if (this != &a) {
//			delete[] m_data;
//			m_data = (a.m_data != 0 ? strcpy(new char[strlen(a.m_data) + 1], a.m_data) : 0);
//		}
//		return *this;
//	}
//	A(A &&a) : m_data(a.m_data) {
//		TRACE("move constructor\n");
//		a.m_data = 0;
//	}
//	A & operator = (A &&a) {
//		TRACE("move assigment\n");
//		if (this != &a) {
//			m_data = a.m_data;
//			a.m_data = 0;
//		}
//		return *this;
//	}
//	~A() { TRACE("destructor\n"); delete[] m_data; }
//private:
//	char * m_data;
//};
//
//void swap(A &a, A &b)
//{
//	A tmp(move(a));
//	a = move(b);
//	b = move(tmp);
//}

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);

	HRESULT hRes = OleInitialize(NULL);

	LoadSeDebugPrivilege();

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	ChatFrame uiImpl;
	uiImpl.Create(NULL, 50, 50, 800, 600);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	OleUninitialize();
	MFShutdown();
	WSACleanup();
	return loopRes;
};