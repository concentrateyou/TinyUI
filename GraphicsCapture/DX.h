#pragma once
#include "DXCommon.h"

namespace GraphicsCapture
{
	DXGI_FORMAT GetDXTextureFormat(DXGI_FORMAT s);
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam);

	HRESULT STDMETHODCALLTYPE DX_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags);
	HRESULT STDMETHODCALLTYPE DX_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags);

	struct TLS
	{
		volatile BOOL		m_map[NUM_BUFFERS];
		volatile INT32		m_current;
		volatile LPVOID		m_data;
		LPBYTE				m_textures[2];
		INT32				m_wait;
	};

	class DX
	{
		DISALLOW_COPY_AND_ASSIGN(DX)
	public:
		DX();
		virtual ~DX();
		BOOL			Initialize();
		void			Uninitialize();
		BOOL			SetWindowsHook();
		void			UnhookWindowsHook();
		BOOL			IsFrameReady(UINT64 interval);
		BOOL			CreateTextureDATA(DWORD dwMapID, DWORD dwSize = sizeof(TextureDATA));
		HookDATA*		GetHookDATA();
		TextureDATA*	GetTextureDATA();
	private:
		BOOL			CreateEvents();
	public:
		HHOOK					m_hhk;
		TinyMutex				m_mutes[2];
		TinyLock				m_lock;
		TinyEvent				m_sourceReady;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_targetReady;
		IO::TinySharedMemory	m_hookDATA;
		IO::TinySharedMemory	m_textureDATA;
	};
	SELECTANY extern DX g_dx;
}
