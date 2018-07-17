#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX11GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX11GraphicsCapture)
	public:
		DX11GraphicsCapture(DX& dx);
		~DX11GraphicsCapture();
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
		TinyComPtr<ID3D11Texture2D>		m_copy2D;
	};
	SELECTANY extern DX11GraphicsCapture g_dx11(g_dx);
}

