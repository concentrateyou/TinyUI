#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DXGIFormat.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DXGI.h>
#include <dwmapi.h>
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
#pragma comment(lib,"Dwmapi.lib")
using namespace DirectX;
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

	struct MATRIXBUFFER
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	typedef BOOL(WINAPI *DwmGetDxSharedSurface)(
		HWND hwnd,
		HANDLE* phSurface,
		LUID* pAdapterLuid,
		ULONG* pFmtWindow,
		ULONG* pPresentFlags,
		ULONGLONG* pWin32kUpdateId);


#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define CAPTURE_EXIT_EVENT      TEXT("CaptureExit")
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define TEXTURE_MUTEX1          TEXT("TextureMutex1")
#define TEXTURE_MUTEX2          TEXT("TextureMutex2")
#define D3DX_PI    (3.14159265358979323846)
#define D3DX_1BYPI ( 1.0 / D3DX_PI )
	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
	BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
