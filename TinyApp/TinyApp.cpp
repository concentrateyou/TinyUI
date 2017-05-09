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
#include "Media/TinyWave.h"
#include "Common/TinyHook.h"
#include "Media/TinyMFMP3Decode.h"
#include "MPG123Decode.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyHTTPRequest.h"
#include "Network/TinyHTTPResponse.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "MediaTest.h"
#include "Common/TinySignal.h";

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

class QSVTest
{
public:
	QSVTest()
	{
		QSV::QSVParam param = m_encode.GetDefaultQSV(640, 360, 1000);
		m_encode.Open(param, BindCallback(&QSVTest::OnDone, this));
		m_rgbaFile.Open("D:\\12345.rgba");
		m_h264File.Create("D:\\12345.h264");
	}
	~QSVTest()
	{
		m_rgbaFile.Close();
		m_h264File.Close();
	}

	void OnDone(BYTE* data, LONG size)
	{
		m_h264File.Write(data, size);
	}
public:
	QSV::QSVEncode m_encode;
	TinyFile m_rgbaFile;
	TinyFile m_h264File;
};

class TestA : public TinyObject
{
	DECLARE_DYNCREATE(TestA)
};

IMPLEMENT_DYNCREATE(TestA, TinyObject)

class TestB : public TestA
{
	DECLARE_DYNCREATE(TestB)
};

IMPLEMENT_DYNCREATE(TestB, TestA)

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
	CMainFrame uiImpl;
	uiImpl.Create(NULL, 10, 10, 800, 600);
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