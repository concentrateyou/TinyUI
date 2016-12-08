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

//CHAR buffer[1024];
//
//void OnReceive(DWORD dwError, TinyUI::Network::AsyncResult* is)
//{
//	if (dwError == 0)
//	{
//		TinyUI::Network::TinySocket* socket = reinterpret_cast<TinyUI::Network::TinySocket*>(is->AsyncState);
//		INT size = socket->EndReceive(is);
//		TinyString str(size + 1);
//		memcpy(str.STR(), buffer, size);
//		str[size] = '\0';
//		TRACE("OnReceive:%s\n", str.STR());
//		socket->BeginReceive(buffer, 1024, 0, BindCallback(&OnReceive), socket);
//
//	}
//	else
//	{
//		TRACE("OnReceive-dwError:%d\n", dwError);
//	}
//
//}
//
//void OnAccept(DWORD dwError, TinyUI::Network::AsyncResult* is)
//{
//	if (dwError == 0)
//	{
//		TRACE("OnAccept-成功\n");
//		TinyUI::Network::TinySocket* socket = reinterpret_cast<TinyUI::Network::TinySocket*>(is->AsyncState);
//		TinyUI::Network::TinySocket* client = socket->EndAccept(is);
//		client->BeginReceive(buffer, 1024, 0, BindCallback(&OnReceive), client);
//	}
//	else
//	{
//		TRACE("OnAccept-dwError:%d\n", dwError);
//	}
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

	//TinyUI::Network::TinyIOServer	ioserver(1);
	//ioserver.Run();

	//TinyUI::Network::TinySocket	socket(&ioserver);
	//socket.Open();
	//socket.Bind(TinyUI::Network::IPEndPoint(TinyUI::Network::IPAddress::IPv4Any(), 5500));
	//socket.Listen();
	//socket.BeginAccept(BindCallback(&OnAccept), &socket);

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

	ioserver.Close();

	OleUninitialize();
	MFShutdown();
	WSACleanup();
	return loopRes;
};