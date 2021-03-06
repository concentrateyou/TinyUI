#pragma once
#include <d3d10_1.h>
#include <d3d11.h>
#include <DXGI.h>
#include <string>
#include "Common/TinyCore.h"
#include "Common/TinyLogging.h"
#include "Common/TinyHook.h"
#include "Common/TinyTime.h"
#include "Common/TinyString.h"
#include "Render/TinyGDI.h"
#include "IO/TinyWorker.h"
#include "IO/TinySharedMemory.h"
#include "DXFramework.h"

using namespace TinyFramework;
using namespace DXFramework;
#pragma comment(lib,"TinyFramework.lib")
#pragma comment(lib,"DXFramework.lib")

namespace GraphicsCapture
{
#define NUM_BUFFERS 3
	const static D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

#define DX_LOCK_STATE	0x00000001
#define DX_ISSUE_STATE	0x00000002

	typedef HRESULT(WINAPI *CREATEDXGIFACTORY1PROC)(REFIID riid, void **ppFactory);
	typedef HRESULT(WINAPI *D3D10CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device**);
	typedef HRESULT(WINAPI *D3D101CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device1**);
	typedef HRESULT(WINAPI*D3D11CREATEPROC)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, D3D_FEATURE_LEVEL*, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
}