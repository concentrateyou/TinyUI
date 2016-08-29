#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include <DXGI.h>
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
#define CAPTURE_EXIT_EVENT      TEXT("CaptureExit")
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
