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
#include "Media/TinyAudioDSPCapture.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Network/TinyHTTPClient.h"
#include "Media/TinyWave.h"

#include "Media/TinyMFTMP3Decode.h"

#include "MPG123Decode.h"

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

Media::TinyWaveFile waveFile;
void OnDecode(BYTE*bits, LONG size, LPVOID ps)
{
	if (size == 0)
		waveFile.Create("D:\\12345.wav", reinterpret_cast<WAVEFORMATEX*>(ps));
	waveFile.Write(bits, size);
}

void OnDecode1(BYTE*bits, LONG size, LPVOID ps)
{
	waveFile.Write(bits, size);
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

	TinyScopedArray<BYTE> rawdata(new BYTE[16384]);

	TinyPerformanceTimer timer;
	Media::TinyMFTMP3Decode mp3;
	timer.BeginTime();
	WAVEFORMATEX wfx = { 0 };
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	mp3.Open((WAVEFORMATEX*)&wfx, 128000, BindCallback(&OnDecode1));
	waveFile.Create("D:\\1234.wav", (WAVEFORMATEX*)&wfx);
	LONGLONG totalSize = 0;
	TinyFile sFile;
	sFile.Open("D:\\KuGou\\梁咏琪 - 胆小鬼.mp3", GENERIC_READ, FILE_SHARE_READ, NULL);
	for (;;)
	{
		DWORD rawsize = sFile.Read(rawdata, 16384);
		mp3.Decode(rawdata, rawsize);
		totalSize += rawsize;
		if (totalSize == sFile.GetSize())
		{
			break;
		}
	}
	mp3.Close();
	timer.EndTime();
	DWORD dwMS = timer.GetMicroseconds() / 1000;
	TRACE("TinyMFTMP3-MS:%d\n", dwMS);
	sFile.Close();

	/*timer.BeginTime();
	Decode::MPG123Decode decode;
	decode.Initialize(BindCallback(&OnDecode));
	decode.Open();
	DWORD totalSize = 0;
	TinyFile sFile;
	sFile.Open("D:\\KuGou\\梁咏琪 - 胆小鬼.mp3", GENERIC_READ, FILE_SHARE_READ, NULL);
	for (;;)
	{
		DWORD rawsize = sFile.Read(rawdata, 16384);
		decode.Decode(rawdata, rawsize);
		totalSize += rawsize;
		if (totalSize == sFile.GetSize())
		{
			break;
		}
	}
	decode.Close();
	timer.EndTime();
	DWORD dwMS = timer.GetMicroseconds() / 1000;
	TRACE("MPG123Decode-MS:%d\n", dwMS);*/

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

	OleUninitialize();
	MFShutdown();
	WSACleanup();
	return loopRes;
};