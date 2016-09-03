#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <DXGI.h>
#include <string>
#include "Common/TinyCore.h"
#include "Common/TinyHook.h"
#include "Common/TinyTime.h"
#include "Common/TinyString.h"
#include "IO/TinySharedMemory.h"
using namespace TinyUI;
#pragma comment(lib,"TinyUI.lib")

namespace DXCapture
{
	typedef HRESULT(WINAPI *CREATEDXGIFACTORY1PROC)(REFIID riid, void **ppFactory);
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define NUM_BUFFERS				3
#define ZERO_ARRAY				{0, 0, 0}
#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define CAPTURE_EXIT_EVENT		TEXT("CaptureExit")
#define CAPTURETYPE_MEMORY      1
#define CAPTURETYPE_SHAREDTEX   2
	typedef struct tagSharedCaptureDATA
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedCaptureDATA;
#pragma pack(push, 8)
	typedef struct tagSharedTextureDATA
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTextureDATA;
#pragma pack(pop)
}