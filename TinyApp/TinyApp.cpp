// TinyApp.cpp : ¶¨ÒåÓ¦ÓÃ³ÌÐòµÄÈë¿Úµã¡£
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

#include "Network/TinySocket.h"

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

//Media::TinyWaveFile waveFile;
//void OnDecode(BYTE*bits, LONG size, LPVOID ps)
//{
//	if (size == 0)
//		waveFile.Create("D:\\12345.wav", reinterpret_cast<WAVEFORMATEX*>(ps));
//	waveFile.Write(bits, size);
//}

//void OnDecode1(BYTE*bits, LONG size, LPVOID ps)
//{
//	waveFile.Write(bits, size);
//}

//Media::TinyWaveFile waveFile;
//
//void OnDecodeMP3(BYTE*bits, LONG size, LPVOID ps)
//{
//	waveFile.Write(bits, size);
//}

#include "MFVideoCapture.h"

Media::TinyWaveFile waveFile;

//void OnAudioDone(BYTE* bits, LONG size, LPVOID ps)
//{
//	if (bits == NULL)
//	{
//		WAVEFORMATEX s = *reinterpret_cast<WAVEFORMATEX*>(ps);
//		waveFile.Create("D:\\12345.wav", &s);
//	}
//	else
//	{
//		waveFile.Write(bits, size);
//	}
//}

//TinyUI::TinyDetour detour;
//typedef INT(WINAPI *MESSAGEBOX)(HWND, LPCSTR, LPCSTR, UINT);
//INT WINAPI MyMessageBox(
//	_In_opt_ HWND hWnd,
//	_In_opt_ LPCSTR lpText,
//	_In_opt_ LPCSTR lpCaption,
//	_In_ UINT uType)
//{
//	MESSAGEBOX ps = (MESSAGEBOX)detour.GetOrig();
//	return ps(NULL,"Ìæ»»ºó", "À²À²À²", MB_OK);
//	//MESSAGEBOX  ps = (MESSAGEBOX)detour.GetOrig();
//	//return ps(NULL, "Ìæ»»ºó", "À²À²À²", MB_OK);
//}
//

//typedef struct tagTCPMsg
//{
//	UINT size;
//	UINT message;
//	CHAR context[256];
//
//	tagTCPMsg()
//	{
//		size = sizeof(tagTCPMsg);
//	}
//
//}TCPMsg;
//
//Network::TinyTCPClient client;
//
//void OnSend(INT error)
//{
//	if (error == 0)
//	{
//
//	}
//	else
//	{
//
//	}
//};
//
//TCPMsg g_msg;
//
//void OnConnect(INT error)
//{
//	if (error == 0)
//	{
//		g_msg.message = 100;
//		string val = "test on connect";
//		memcpy(g_msg.context, &val[0], val.size());
//		client.Send((CHAR*)&g_msg, g_msg.size, BindCallback(&OnSend));
//	}
//}

Network::TinySocket g_socket;

CHAR m_msg[1024] = { 0 };

void ReceiveCallback(DWORD dwError, Network::AsyncResult* async)
{
	Network::TinySocket::StreamAsyncResult* result = reinterpret_cast<Network::TinySocket::StreamAsyncResult*>(async);
	TRACE("ReceiveCallback:%d,%s\n", result->BytesTransferred, m_msg);

	if (!g_socket.BeginReceive(m_msg, 1024, 0, BindCallback(&ReceiveCallback), &g_socket))
	{
		INT a = 0;
	}
}

void SendCallback(DWORD dwError, Network::AsyncResult* async)
{
	Network::TinySocket::StreamAsyncResult* result = reinterpret_cast<Network::TinySocket::StreamAsyncResult*>(async);
	TRACE("SendCallback:%d\n", result->BytesTransferred);
}

void ConnectCallback(DWORD dwError, Network::AsyncResult* async)
{
	string msg = "test send";
	memcpy(m_msg, &msg[0], msg.size());
	if (!g_socket.BeginSend(m_msg, 1024, 0, BindCallback(&SendCallback), &g_socket))
	{
		INT a = 0;
	}
	if (!g_socket.BeginReceive(m_msg, 1024, 0, BindCallback(&ReceiveCallback), &g_socket))
	{
		INT a = 0;
	}
}

void AcceptCallback(DWORD dwError, Network::AsyncResult* async)
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


	/*vector<MF::MFVideoCapture::Name> names;
	MF::MFVideoCapture::GetDevices(names);
	vector<MF::MFVideoCaptureParam> params;
	MF::MFVideoCapture::GetDeviceParams(names[0], params);*/

	//TinyScopedArray<BYTE> rawdata(new BYTE[16384]);
	//TinyPerformanceTimer timer;
	//Media::TinyMFTMP3Decode mp3;
	//timer.BeginTime();
	//WAVEFORMATEX wfx = { 0 };
	//wfx.cbSize = 0;
	//wfx.wFormatTag = WAVE_FORMAT_PCM;
	//wfx.nChannels = 2;
	//wfx.nSamplesPerSec = 44100;
	//wfx.wBitsPerSample = 16;
	//wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	//wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	//mp3.Open((WAVEFORMATEX*)&wfx, 128000, BindCallback(&OnDecode1));
	//waveFile.Create("D:\\1234.wav", (WAVEFORMATEX*)&wfx);
	//LONGLONG totalSize = 0;
	//TinyFile sFile;
	//sFile.Open("D:\\KuGou\\ÁºÓ½ç÷ - µ¨Ð¡¹í.mp3", GENERIC_READ, FILE_SHARE_READ, NULL);
	//for (;;)
	//{
	//	DWORD rawsize = sFile.Read(rawdata, 16384);
	//	mp3.Decode(rawdata, rawsize);
	//	totalSize += rawsize;
	//	if (totalSize == sFile.GetSize())
	//	{
	//		break;
	//	}
	//}
	//mp3.Close();
	//timer.EndTime();
	//DWORD dwMS = timer.GetMicroseconds() / 1000;
	//TRACE("TinyMFTMP3-MS:%d\n", dwMS);
	//sFile.Close();

	/*timer.BeginTime();
	Decode::MPG123Decode decode;
	decode.Initialize(BindCallback(&OnDecode));
	decode.Open();
	DWORD totalSize = 0;
	TinyFile sFile;
	sFile.Open("D:\\KuGou\\ÁºÓ½ç÷ - µ¨Ð¡¹í.mp3", GENERIC_READ, FILE_SHARE_READ, NULL);
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

	/*BOOL bRes = FALSE;
	Media::TinyMP3File	mp3File;
	bRes = mp3File.Open("D:\\Íõ·Æ - ´Ò´ÒÄÇÄê.mp3");
	WAVEFORMATEX sMFT = mp3File.GetFormat()->wfx;
	sMFT.wFormatTag = WAVE_FORMAT_PCM;
	sMFT.wBitsPerSample = 16;
	sMFT.nBlockAlign = (sMFT.nChannels * sMFT.wBitsPerSample) / 8;
	sMFT.nAvgBytesPerSec = sMFT.nSamplesPerSec * sMFT.nBlockAlign;
	sMFT.cbSize = 0;
	bRes = waveFile.Create("D:\\Íõ·Æ - ´Ò´ÒÄÇÄê.wav", &sMFT);
	Media::TinyMFMP3Decode decode;
	bRes = decode.Open(mp3File.GetFormat(), BindCallback(&OnDecodeMP3));
	LONG nNumberOfBytesRead = 0;
	LONGLONG timestamp = 0;
	TinyScopedArray<BYTE> bits(new BYTE[mp3File.GetMaxOutputBytes()]);
	for (;;)
	{
		bRes = mp3File.Read(bits, mp3File.GetMaxOutputBytes(), &nNumberOfBytesRead, timestamp);
		if (!bRes)
			break;
		bRes = decode.Decode(bits, nNumberOfBytesRead);
	}
	decode.Close();*/

	//detour.Initialize((FARPROC)&MessageBox, (FARPROC)&MyMessageBox);
	//detour.BeginDetour();
	//MessageBox(NULL, "Ìæ»»Ç°", "À²À²À²", MB_OK);
	//detour.EndDetour();
	//detour.Uninitialize();
	//MessageBox(NULL, "Ìæ»»Ç°", "À²À²À²", MB_OK);

	if (g_socket.Open())
	{
		if (g_socket.BeginConnect(Network::IPEndPoint(Network::IPAddress("127.0.0.1"), 5400), BindCallback(&ConnectCallback), &g_socket))
		{

		}
		/*if (g_socket.Bind(Network::IPEndPoint(Network::IPAddress("127.0.0.1"), 5400)))
		{
			if (g_socket.Listen(10))
			{
				if (g_socket.BeginAccept(BindCallback(&AcceptCallback), &g_socket))
				{
					INT a = 0;
				}
			}
		}*/
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