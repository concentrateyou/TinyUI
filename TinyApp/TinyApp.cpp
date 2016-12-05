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

#include <functional>

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

//CHAR buffer[10];
//string val;
//
//void OnRecv(DWORD errorCode, DWORD dwBytes, LPVOID key)
//{
//	string str;
//	str.resize(dwBytes);
//	memcpy(&str[0], buffer, dwBytes);
//	val += str;
//	TinyUI::Network::TinySocket* socket = reinterpret_cast<TinyUI::Network::TinySocket*>(key);
//	TinyUI::Network::CompleteCallback cb = BindCallback(&OnRecv);
//	socket->BeginReceive(buffer, 10, 0, cb, key);
//}
//
//void OnConnect(DWORD errorCode, DWORD dwBytes, LPVOID key)
//{
//	TinyUI::Network::TinySocket* socket = reinterpret_cast<TinyUI::Network::TinySocket*>(key);
//	TinyUI::Network::CompleteCallback cb = BindCallback(&OnRecv);
//	socket->BeginReceive(buffer, 10, 0, cb, key);
//}
//CHAR buffer[1024];
//
//void OnReceive(DWORD errorCode, TinyUI::Network::AsyncResult* result)
//{
//	if (errorCode != 0)
//		return;
//	TinyUI::Network::TinySocket* client = reinterpret_cast<TinyUI::Network::TinySocket*>(result->AsyncState);
//	INT size = client->EndReceive(result);
//	string str;
//	str.resize(size + 1);
//	memcpy(&str[0], buffer, size);
//	client->Close();
//	SAFE_DELETE(client);
//}
//void OnAccept(DWORD errorCode, TinyUI::Network::AsyncResult* result)
//{
//	if (errorCode != 0)
//		return;
//	TinyUI::Network::TinySocket* listen = reinterpret_cast<TinyUI::Network::TinySocket*>(result->AsyncState);
//	TinyUI::Network::TinySocket* client = listen->EndAccept(result);
//	TinyUI::Network::CompleteCallback cb = BindCallback(&OnReceive);
//	BOOL bRes = client->BeginReceive(buffer, 1024, 0, cb, client);
//	if (!bRes)
//	{
//		TRACE("BeginReceive-ERROR:%d\n",WSAGetLastError());
//	}
//}

void OnAccept(DWORD errorCode, TinyUI::Network::AsyncResult* result)
{

}


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