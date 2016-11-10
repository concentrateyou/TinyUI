#pragma once
#include "DXCommon.h"
#include <d3d9.h>

namespace DXCapture
{
	typedef IDirect3D9* (WINAPI*D3D9CREATEPROC)(UINT);
	typedef HRESULT(WINAPI*D3D9CREATEEXPROC)(UINT, IDirect3D9Ex**);
	/// <summary>
	/// XX9纹理数据捕获
	/// </summary>
	class DX9Capture
	{
	public:
		DX9Capture();
		~DX9Capture();
		BOOL Initialize(HWND hWND);
		BOOL Render(IDirect3DDevice9 *device);
		void Reset();
		void Setup(IDirect3DDevice9 *pThis);
		BOOL DX9GPUHook(IDirect3DDevice9 *device);
		static DX9Capture& Instance();
	private:
		BOOL BuildEvents();
	private:
		static LRESULT CALLBACK CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam);
		static HRESULT STDMETHODCALLTYPE DX9EndScene(IDirect3DDevice9 *pThis);
		static HRESULT STDMETHODCALLTYPE DX9Present(IDirect3DDevice9 *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		static HRESULT STDMETHODCALLTYPE DX9PresentEx(IDirect3DDevice9Ex *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE DX9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE DX9Reset(IDirect3DDevice9 *pThis, D3DPRESENT_PARAMETERS *params);
		static HRESULT STDMETHODCALLTYPE DX9ResetEx(IDirect3DDevice9Ex *pThis, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);
	public:
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		INT								m_patchType;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		TinyLock						m_lock;
		TinyComPtr<ID3D10Device1>		m_d3d10;
		TinyComPtr<IDirect3DSurface9>	m_dX9TextureSurface;
		TinyDetour						m_dX9EndScene;
		TinyDetour						m_dX9Reset;
		TinyDetour						m_dX9ResetEx;
		TinyDetour						m_dX9Present;
		TinyDetour						m_dX9PresentEx;
		TinyDetour						m_dX9SwapPresent;
		TinyEvent						m_start;
		TinyEvent						m_stop;
		TinyEvent						m_ready;
		TinyEvent						m_exit;
		TinyScopedLibrary				m_d3d9;
		TinyScopedLibrary				m_d3d10_1;
		TinyScopedLibrary				m_dxgi;
		IO::TinySharedMemory			m_memery;
		IO::TinySharedMemory			m_textureMemery;
		HHOOK							m_hhk;
	};
}

