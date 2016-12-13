// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "Windowless/TinyVisualHWND.h"
#include "Windowless/TinyVisualRichText.h"
#include "Render/TinyDDraw.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
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

TinyUI::Media::TinyWaveFile waveFile;
WAVEFORMATEX* waveFormat;

void CaptureCB(BYTE* bits, LONG size, LPVOID ps)
{
	waveFile.Write(bits, size * waveFormat->nBlockAlign);
}

void ReadCB(BYTE* bits, LONG size, LPVOID ps)
{
	LONG numberOfBytesRead = 0;
	waveFile.Read(bits, size, &numberOfBytesRead);
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

	//waveFile.Open("D:\\1234.wav");
	//vector<Media::TinyWASAPIAudioRender::Name> names;
	//TinyUI::Media::TinyWASAPIAudioRender::GetDevices(eRender, names);
	//TinyUI::Media::TinyWASAPIAudioRender render;
	//render.Initialize(BindCallback(&ReadCB), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, AUDCLNT_SHAREMODE_SHARED);
	//BOOL bRes = render.Open(names[0], waveFile.GetFormat());
	//bRes = render.Start();

	/*vector<Media::TinyWASAPIAudioRender::Name> names;
	TinyUI::Media::TinyWASAPIAudioRender::GetDevices(eRender, names);
	TinyUI::Media::TinyWASAPIAudioCapture capture;
	capture.Initialize(BindCallback(&CaptureCB), AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST);
	BOOL bRes = capture.Open(names[0], NULL);
	waveFormat = capture.GetInputFormat();
	waveFile.Create("D:\\1234.wav", waveFormat);
	bRes = capture.Start();*/

	/*vector<Media::TinyWASAPIAudioCapture::Name> names;
	TinyUI::Media::TinyWASAPIAudioCapture::GetDevices(eCapture, names);
	TinyUI::Media::TinyWASAPIAudioCapture capture;
	capture.Initialize(BindCallback(&CaptureCB), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST);
	BOOL bRes = capture.Open(names[0], NULL);
	waveFormat = capture.GetInputFormat();
	waveFile.Create("D:\\12345.wav", waveFormat);
	bRes = capture.Start();*/

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	CMainFrame uiImpl;
	uiImpl.Create(NULL, 50, 50, 800, 600);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	//capture.Close();
	//render.Close();

	OleUninitialize();
	MFShutdown();
	WSACleanup();
	return loopRes;
};