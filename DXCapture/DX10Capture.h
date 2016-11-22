#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	typedef HRESULT(WINAPI *D3D10CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, IUnknown**);

	class DX10Capture
	{
	public:
		DX10Capture();
		~DX10Capture();
	};
}

