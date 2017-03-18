#pragma once
#include "DX.h"


namespace DXCapture
{
	class DX101Capture
	{
	public:
		DX101Capture(DX& dx);
		~DX101Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
	public:
		DX&								m_dx;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D101;
		DXGI_FORMAT						m_dxgiFormat;
		SharedCaptureDATA				m_captureDATA;
		TinyComPtr<ID3D10Resource>		m_resource;
	};
	SELECTANY extern DX101Capture g_dx101(g_dx);
}

