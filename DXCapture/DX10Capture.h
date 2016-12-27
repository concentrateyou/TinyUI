#pragma once
#include "DX.h"

namespace DXCapture
{
	HRESULT STDMETHODCALLTYPE DX10_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX10_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	class DX10Capture
	{
	public:
		DX10Capture(DX& dx);
		~DX10Capture();
		BOOL Initialize(HWND hWND);
		void Reset(BOOL bRelease = TRUE);
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX10GPUHook(ID3D10Device *device);
	public:
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D10;
		TinyLock						m_lock;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX10Capture g_dx10(g_dx);
}

