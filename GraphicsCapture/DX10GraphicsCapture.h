#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX10GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX10GraphicsCapture)
	public:
		DX10GraphicsCapture(DX& dx);
		~DX10GraphicsCapture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX10GPUHook(ID3D10Device *device);
	public:
		HookDATA						m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		HANDLE							m_handle;
		HMODULE							m_hD3D10;
		TinyComPtr<ID3D10Texture2D>		m_texture2D;
		TinyComPtr<ID3D10Texture2D>		m_copy2D;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX10GraphicsCapture g_dx10(g_dx);
}

