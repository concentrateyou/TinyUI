#pragma once
#include "DXCommon.h"
#include "DX10Capture.h"
#include "DX101Capture.h"
#include "DX11Capture.h"

namespace DXCapture
{
	class DXGICapture
	{
	public:
		DXGICapture(DX10Capture& dx10, DX101Capture& dx101, DX11Capture& dx11);
		~DXGICapture();
		BOOL Initialize(HWND hWND);
		void Reset(BOOL bRelease = TRUE);
	private:
		BOOL m_bDX10;
		BOOL m_bDX101;
		BOOL m_bDX11;
		DX10Capture& m_dx10;
		DX101Capture& m_dx101;
		DX11Capture& m_dx11;
	};

	SELECTANY extern DXGICapture g_dxgi(g_dx10, g_dx101, g_dx11);
}

