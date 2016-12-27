#pragma once
#include "DX.h"

namespace DXCapture
{
	HRESULT STDMETHODCALLTYPE DX101_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX101_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	class DX101Capture
	{
	public:
		DX101Capture(DX& dx);
		~DX101Capture();
		BOOL Initialize(HWND hWND);
		void Reset(BOOL bRelease = TRUE);
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
	public:
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D101;
		TinyLock						m_lock;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX101Capture g_dx101(g_dx);
}

