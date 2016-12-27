#pragma once
#include "DX.h"

namespace DXCapture
{
	HRESULT STDMETHODCALLTYPE DX11_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX11_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	class DX11Capture
	{
	public:
		DX11Capture(DX& dx);
		~DX11Capture();
		BOOL Initialize(HWND hWND);
		void Reset(BOOL bRelease = TRUE);
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *pThis, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *pThis);
	public:
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D11;
		TinyLock						m_lock;
		TinyComPtr<ID3D11Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX11Capture g_dx11(g_dx);
}

