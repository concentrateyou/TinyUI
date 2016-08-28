#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <D3DX10Tex.h>
#include <DXGI.h>
#include <D3dx9math.h>
#include <TlHelp32.h>
#include <string>
#include "Common/TinyCommon.h"
#include "Control/TinyControl.h"
#include "Render/TinyGDI.h"
#include "IO/TinySharedMemory.h"
using namespace TinyUI;

namespace DXFramework
{
	typedef struct tagWNDINFO
	{
		HWND	hWND;
		DWORD	dwProcessID;
		DWORD	dwThreadID;
		HANDLE	hProcess;
	}WNDINFO, *LPWNDINFO;

#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define APP_EXIT_EVENT          TEXT("AppExit")
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
