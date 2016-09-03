#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DXGIFormat.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include <DXGI.h>
#include <TlHelp32.h>
#include <string>
#include "Common/TinyCommon.h"
#include "Control/TinyControl.h"
#include "Render/TinyGDI.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTask.h"
using namespace TinyUI;
using namespace std;

namespace DXFramework
{
	typedef struct tagWNDINFO
	{
		CHAR	className[MAX_PATH];
		CHAR	exeName[MAX_PATH];
		HWND	hWND;
		DWORD	dwProcessID;
		DWORD	dwThreadID;
		HANDLE	hProcess;
	}WNDINFO, *LPWNDINFO;

#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define CAPTURE_EXIT_EVENT      TEXT("CaptureExit")
#define WINDOW_CLOSE_EVENT      TEXT("WINDOW_CLOSE")
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
