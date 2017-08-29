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
#include "FLVParser.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyHTTPRequest.h"
#include "Network/TinyHTTPResponse.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "Common/TinySignal.h"
#include "SkinWindow.h"
#include "Media/TinyWave.h"
#include "FLVParser.h"

#include "Media/TinySoundCapture.h"
#include "QSVEncoder.h"
#include <fstream>

using namespace TinyUI;
using namespace TinyUI::Network;
using namespace TinyUI::Media;
using namespace Decode;

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

	//QSV::QSVEncoder encoder;
	//encoder.Open({ 1280,720 }, { 1280,720 });
	//FILE *hFile1 = fopen("D:\\output.rgb4", "rb");
	//FILE *hFile2 = fopen("D:\\test2.h264", "wb+");
	//BYTE* pArray = new BYTE[1024 * 1024 * 5];
	//LONG offset = 0;
	//for (;;)
	//{
	//	SampleTag tag;
	//	tag.bits = pArray;
	//	LONG size = fread(pArray, 1, 1280 * 720 * 4, hFile1);
	//	if (size == 0)
	//		break;
	//	tag.size = size;
	//	TinyPerformanceTimer timer;
	//	timer.BeginTime();
	//	BYTE* bo = NULL;
	//	/*LONG  so = 0;
	//	encoder.Encode(tag, bo, so);
	//	if (so > 0)
	//	{
	//		timer.EndTime();
	//		TRACE("Time:%lld\n", timer.GetMillisconds());
	//		fwrite(bo, 1, so, hFile2);
	//	}*/
	//}
	//fclose(hFile1);
	//fclose(hFile2);

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	SkinWindow uiImpl;
	uiImpl.Create(NULL, "D:\\Develop\\TinyUI\\TinyUI\\resource.xml");
	//uiImpl.Create(NULL, 10, 10, 100, 100);
	//uiImpl.CenterWindow(NULL, { 800,600 });
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	OleUninitialize();
	MFShutdown();
	WSACleanup();

	return loopRes;
};