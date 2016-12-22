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

#include "Media/TinyMFTMP3.h"

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
	timer.BeginTime();
	Media::TinyMFTMP3 mp3;
	MPEGLAYER3WAVEFORMAT sFMT;
	ZeroMemory(&sFMT, sizeof(sFMT));
	sFMT.wID = MPEGLAYER3_ID_MPEG;
	sFMT.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
	sFMT.nBlockSize = WORD(144 * (128000 / 44100));
	sFMT.nFramesPerBlock = 1;
	sFMT.nCodecDelay = 0;
	sFMT.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	sFMT.wfx.nChannels = 2;
	sFMT.wfx.nSamplesPerSec = 44100;
	sFMT.wfx.wBitsPerSample = 16;
	sFMT.wfx.nBlockAlign = (sFMT.wfx.nChannels * sFMT.wfx.wBitsPerSample) / 8;
	sFMT.wfx.nAvgBytesPerSec = sFMT.wfx.nSamplesPerSec * sFMT.wfx.nBlockAlign;
	sFMT.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
	mp3.Open((WAVEFORMATEX*)&sFMT, BindCallback(&OnDecode1));

	WAVEFORMATEX wfx = { 0 };
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (sFMT.wfx.nChannels * sFMT.wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = sFMT.wfx.nSamplesPerSec * sFMT.wfx.nBlockAlign;
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
	timer.BeginTime();
	Decode::MPG123Decode decode;
	decode.Initialize(BindCallback(&OnDecode));
	decode.Open();
	totalSize = 0;
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
	dwMS = timer.GetMicroseconds() / 1000;
	TRACE("MPG123Decode-MS:%d\n", dwMS);

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