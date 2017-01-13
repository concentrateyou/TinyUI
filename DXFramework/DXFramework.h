#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DXGIFormat.h>
#include <D3DX10math.h>
#include <D3DX11tex.h>
#include <DXGI.h>
#include <TlHelp32.h>
#include <string>
#include "Common/TinyCommon.h"
#include "Common/TinyUtility.h"
#include "Common/TinyTime.h"
#include "Common/TinyLogging.h"
#include "Control/TinyControl.h"
#include "Render/TinyGDI.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
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
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
	BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
