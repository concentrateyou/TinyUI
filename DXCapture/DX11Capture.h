#pragma once
#include "DX.h"

namespace DXCapture
{
	class DX11Capture
	{
	public:
		DX11Capture(DX& dx);
		~DX11Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *pThis, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *pThis);
	public:
		DX&								m_dx;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D11;
		DXGI_FORMAT						m_dxgiFormat;
		SharedCaptureDATA				m_captureDATA;
		TinyComPtr<ID3D11Resource>		m_resource;
	};
	SELECTANY extern DX11Capture g_dx11(g_dx);
}

