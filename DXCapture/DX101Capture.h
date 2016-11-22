#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	typedef HRESULT(WINAPI *D3D101CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, IUnknown**);

	class DX101Capture
	{
	public:
		DX101Capture();
		~DX101Capture();
	};
}

