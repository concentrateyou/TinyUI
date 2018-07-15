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
#include <d2d1.h>
#include <wincodec.h>
#include <d2d1helper.h>
#include "Common/TinyCommon.h"
#include "Common/TinyUtility.h"
#include "Common/TinyTime.h"
#include "Common/TinyLogging.h"
#include "Control/TinyControl.h"
#include "Common/TinyString.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyThread.h"
#include "IO/TinyRingQueue.h"
#include "Render/TinyGDI.h"
#pragma comment(lib,"Dwmapi.lib")
using namespace DirectX;
using namespace TinyFramework;
using namespace std;

namespace DXFramework
{
	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0,
		hitTopRight = 1,
		hitBottomRight = 2,
		hitBottomLeft = 3,
		hitTop = 4,
		hitRight = 5,
		hitBottom = 6,
		hitLeft = 7,
		hitMiddle = 8
	};

	enum D3DX11_IMAGE_FILE_FORMAT
	{
		BMP,
		JPG,
		PNG,
		TIFF,
		GIF,
		WMP
	};

	REFGUID GetWICCodec(D3DX11_IMAGE_FILE_FORMAT format);

	typedef struct tagWNDINFO
	{
		CHAR	className[MAX_PATH];
		CHAR	exeName[MAX_PATH];
		HWND	hWND;
		DWORD	dwProcessID;
		DWORD	dwThreadID;
		HANDLE	hProcess;
	}WNDINFO, *LPWNDINFO;

	typedef struct tagPROCESSINFO
	{
		DWORD	dwProcessID;
		HANDLE	hProcess;
	}PROCESSINFO, *LPPROCESSINFO;

	struct MATRIXBUFFER
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct TRANSPARENTBUFFER
	{
		FLOAT alpha;
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
	BOOL WINAPI ProcessExists(const TinyString& exeName, PROCESSINFO& ps);
	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
	BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
