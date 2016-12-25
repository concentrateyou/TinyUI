#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	class DX101Capture
	{
	public:
		DX101Capture();
		~DX101Capture();
		BOOL Initialize(HWND hWND);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
		static DX101Capture& Instance();
	private:
		BOOL BuildEvents();
		static LRESULT CALLBACK CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam);
		static HRESULT STDMETHODCALLTYPE DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
		static HRESULT STDMETHODCALLTYPE DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);
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
		TinyEvent						m_start;
		TinyEvent						m_stop;
		TinyEvent						m_ready;
		TinyEvent						m_exit;
		IO::TinySharedMemory			m_memery;
		IO::TinySharedMemory			m_textureMemery;
		HHOOK							m_hhk;
	};
}

