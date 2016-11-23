#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	class DX11Capture
	{
	public:
		DX11Capture();
		~DX11Capture();
		BOOL Initialize(HWND hWND);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *pThis, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *pThis);
		static DX11Capture& Instance();
	private:
		BOOL BuildEvents();
		/*BOOL SaveBitmap(LPCSTR pzFile);*/
		static LRESULT CALLBACK CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam);
		static HRESULT STDMETHODCALLTYPE DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
		static HRESULT STDMETHODCALLTYPE DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);
	public:
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		TinyLock						m_lock;
		TinyComPtr<ID3D11Resource>		m_resource;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		TinyEvent						m_start;
		TinyEvent						m_stop;
		TinyEvent						m_ready;
		TinyEvent						m_exit;
		TinyScopedLibrary				m_d3d11;

	/*	TinyScopedLibrary				m_d3d10_1;
		TinyScopedLibrary				m_dxgi;
		TinyComPtr<ID3D10Device1>		m_device;*/

		IO::TinySharedMemory			m_memery;
		IO::TinySharedMemory			m_textureMemery;
		HHOOK							m_hhk;
	};
}

