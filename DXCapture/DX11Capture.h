#pragma once
#include "DXCommon.h"
#include <d3d11.h>
#include <D3DX11tex.h>

namespace DXCapture
{
	typedef HRESULT(WINAPI*D3D11CREATEPROC)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, D3D_FEATURE_LEVEL*, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, IUnknown**, D3D_FEATURE_LEVEL*, IUnknown**);

	class DX11Capture
	{
	public:
		DX11Capture();
		~DX11Capture();
		BOOL Initialize(HWND hWND);
		BOOL Render(ID3D11Device *device);
		void Reset();
		void Setup(ID3D11Device *pThis);
		BOOL DX10GPUHook(ID3D11Device *device);
		static DX11Capture& Instance();
	private:
		BOOL BuildEvents();
		static LRESULT CALLBACK CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam);
		static HRESULT STDMETHODCALLTYPE DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
		static HRESULT STDMETHODCALLTYPE DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);
	public:
		DXGI_FORMAT						m_dxgiFormat;
		INT								m_patchType;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		TinyLock						m_lock;
		TinyComPtr<ID3D10Device1>		m_d3d10;
		TinyComPtr<IDXGISwapChain>		m_swap;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		TinyEvent						m_start;
		TinyEvent						m_stop;
		TinyEvent						m_ready;
		TinyEvent						m_exit;
		TinyScopedLibrary				m_d3d11;
		TinyScopedLibrary				m_d3d10_1;
		IO::TinySharedMemory			m_memery;
		IO::TinySharedMemory			m_textureMemery;
		HHOOK							m_hhk;
	};
}

