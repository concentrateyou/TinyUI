#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <d3d11.h>
#include <D3DX11tex.h>
#include <DXGI.h>
#include <string>
#include "Common/TinyCore.h"
#include "Common/TinyHook.h"
#include "Common/TinyTime.h"
#include "Common/TinyString.h"
#include "IO/TinySharedMemory.h"
#include "Common/TinyLogging.h"
using namespace TinyUI;
#pragma comment(lib,"TinyUI.lib")
#pragma comment(lib,"D3DX11.lib")

namespace DXCapture
{
	typedef HRESULT(WINAPI *CREATEDXGIFACTORY1PROC)(REFIID riid, void **ppFactory);
#define SHAREDCAPTURE_MEMORY	TEXT("Local\\SharedCaptureMemory")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define CAPTURE_EXIT_EVENT		TEXT("CaptureExit")
#define CAPTURETYPE_MEMORY      1
#define CAPTURETYPE_SHAREDTEX   2

	typedef HRESULT(WINAPI *D3D10CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device**);
	typedef HRESULT(WINAPI *D3D101CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device1**);
	typedef HRESULT(WINAPI*D3D11CREATEPROC)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, D3D_FEATURE_LEVEL*, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

	typedef struct tagSharedCaptureDATA
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		BOOL		bMultisample;
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