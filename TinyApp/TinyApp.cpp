// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "MFFrame.h"
#include "Windowless/TinyVisualHWND.h"
#include "Windowless/TinyVisualRichText.h"
#include "Render/TinyDDraw.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Network/TinyHTTPClient.h"
#include "Media/TinyWave.h"
#include "Common/TinyHook.h"
#include "Media/TinyMFMP3Decode.h"
#include "MPG123Decode.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyHTTPClient.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"

using namespace TinyUI;
using namespace TinyUI::Network;

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

	Network::TinyHTTPClient client;

	Network::TinyDNS dns;
	AddressList list;
	dns.Resolver("www.baidu.com", "http", list);
	if (list.size() > 0)
	{
		client.Connect(list[0]);
	}



	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	CMainFrame uiImpl;
	uiImpl.Create(NULL, 50, 50, 800, 800);
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