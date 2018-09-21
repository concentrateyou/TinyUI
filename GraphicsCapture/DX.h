#pragma once
#include "DXCommon.h"

namespace GraphicsCapture
{
	DXGI_FORMAT GetDX10PlusTextureFormat(DXGI_FORMAT s);
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam);

	HRESULT STDMETHODCALLTYPE DX_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	class DX
	{
		DISALLOW_COPY_AND_ASSIGN(DX)
	public:
		DX();
		virtual ~DX();
		BOOL Initialize();
		void Uninitialize();
		BOOL SetWindowsHook();
		void UnhookWindowsHook();
		HookDATA* GetHookDATA();
		SharedTextureDATA* GetSharedTextureDATA(DWORD dwSize = sizeof(SharedTextureDATA));
		BYTE* GetSharedTexture(DWORD dwSize = sizeof(SharedTextureDATA));
	private:
		BOOL CreateEvents();
	public:
		HHOOK					m_hhk;
		TinyMutex				m_mutes[2];
		TinyEvent				m_sourceReady;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_targetReady;
		IO::TinySharedMemory	m_hookDATA;
		IO::TinySharedMemory	m_textureDATA;
	};
	SELECTANY extern DX g_dx;
}
