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
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *swap);
		BOOL DX11CPUHook(ID3D11Device *swap);
	public:
		DX&								m_dx;
		volatile BOOL					m_bActive;
		volatile BOOL					m_bCapturing;
		HANDLE							m_handle;
		HMODULE							m_hD3D11;
		DXGI_FORMAT						m_dxgiFormat;
		TinyComPtr<ID3D11Texture2D>		m_texture2D;
	};
	SELECTANY extern DX11GraphicsCapture g_dx11(g_dx);
}

