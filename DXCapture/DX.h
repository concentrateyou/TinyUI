#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	DXGI_FORMAT GetDX10PlusTextureFormat(DXGI_FORMAT s);
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam);

	HRESULT STDMETHODCALLTYPE DX_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	class DX
	{
	public:
		DX();
		virtual ~DX();
		BOOL Initialize();
		void Uninitialize();
		BOOL SetWindowsHook();
		void UnhookWindowsHook();
		SharedCaptureDATA* GetSharedCaptureDATA();
		SharedTextureDATA* GetSharedTextureDATA();
	private:
		BOOL BuildEvents();
	public:
		HHOOK					m_hhk;
		TinyEvent				m_wait;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_ready;
		TinyEvent				m_exit;
		IO::TinySharedMemory	m_captureMemery;
		IO::TinySharedMemory	m_textureMemery;
	};
	SELECTANY extern DX g_dx;
}
