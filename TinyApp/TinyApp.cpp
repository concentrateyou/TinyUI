// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "ChatDlg.h"
#include "MFFrame.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualContextMenu.h"
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
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "Common/TinySignal.h"
#include "SkinWindow.h"
#include "Media/TinyWave.h"
#include "FLVParser.h"
#include "TSReader.h"
#include "Windowless/TinyVisualLayeredWindow.h"
#include "Media/TinySoundCapture.h"
#include "QSVEncoder.h"
#include "MediaTest.h"
#include "Media/TinyMFIntelQSVDecode.h"
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
	CoInitialize(NULL);

	FILE* hFile1 = NULL;
	fopen_s(&hFile1, "D:\\test.264", "wb+");

	FILE* hFile2 = NULL;
	fopen_s(&hFile2, "D:\\test.aac", "wb+");

	TSReader reader;
	reader.OpenFile("D:\\1.ts");
	for (;;)
	{
		TS_BLOCK block;
		if (!reader.ReadBlock(block))
		{
			break;
		}
		if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			fwrite(block.video.data, 1, block.video.size, hFile1);
		}
		if (block.streamType == TS_STREAM_TYPE_AUDIO_AAC)
		{
			fwrite(block.audio.data, 1, block.audio.size, hFile2);
		}
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
	fclose(hFile1);
	fclose(hFile2);
	//MediaTest test;
	//test.H264ToI420("D:\\Media\\test.264", "D:\\Media\\test.yuv");
	//test.MP3ToWave("D:\\Media\\李玉刚-刚好遇见你.mp3", "D:\\Media\\李玉刚-刚好遇见你.wav");
	/*FILE* hFile = NULL;
	fopen_s(&hFile, "D:\\test.264", "rb");
	TinyMFIntelQSVDecode decoder;
	decoder.Open({ 1280,720 }, 25);
	INT pos = 0;
	TinyScopedArray<BYTE> bits(new BYTE[1024 * 64]);
	for (;;)
	{
		size_t count = fread(bits, 1, 1024 * 64, hFile);
		if (count == 0)
			break;
		BYTE* bo = NULL;
		DWORD so = 0;
		SampleTag tag = { 0 };
		tag.bits = bits;
		tag.size = count;
		decoder.Decode(tag, bo, so);
	}
	fclose(hFile);*/

	CoUninitialize();

	/*TinyVisualResource::GetInstance().Load("skin\\resource.xml");
	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	TinyVisualLayeredWindow uiImpl;
	string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\contextmenu.xml");
	uiImpl.Create(NULL, szFile.c_str());
	TinyVisualContextMenu* contextmenu = (TinyVisualContextMenu*)uiImpl.GetDocument().GetVisualByName("menu1");
	contextmenu->Add("Test1");
	contextmenu->Add("Test2");
	contextmenu->Add("Test3");
	uiImpl.Update();*/
	/*INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();*/

	OleUninitialize();
	MFShutdown();
	WSACleanup();

	return 0;
};