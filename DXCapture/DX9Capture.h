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
		void Uninitialize();
		BOOL Render(IDirect3DDevice9 *device);
		void Reset();
		BOOL D3D9GPUHook(IDirect3DDevice9 *device);
		static DX9Capture& Instance();
	private:
		static HRESULT STDMETHODCALLTYPE D3D9EndScene(IDirect3DDevice9 *pThis);
		static HRESULT STDMETHODCALLTYPE D3D9Present(IDirect3DDevice9 *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		static HRESULT STDMETHODCALLTYPE D3D9PresentEx(IDirect3DDevice9Ex *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9Reset(IDirect3DDevice9 *pThis, D3DPRESENT_PARAMETERS *params);
		static HRESULT STDMETHODCALLTYPE D3D9ResetEx(IDirect3DDevice9Ex *pThis, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);
	public:
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		INT								m_patchType;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		DWORD							m_dwCurrentCapture;
		HANDLE							m_hTextureHandle;
		TinyLock						m_lock;
		TinyComPtr<ID3D10Device1>		m_d3d10Device1;
		TinyComPtr<IDirect3DSurface9>	m_d3d9TextureSurface;
		TinyDetour						m_d3d9EndScene;
		TinyDetour						m_d3d9Reset;
		TinyDetour						m_d3d9ResetEx;
		TinyDetour						m_d3d9Present;
		TinyDetour						m_d3d9PresentEx;
		TinyDetour						m_d3d9SwapPresent;
		TinyEvent						m_start;
		TinyEvent						m_stop;
		TinyEvent						m_ready;
		TinyScopedLibrary				m_d3d9;
		TinyScopedLibrary				m_d3d10_1;
		TinyScopedLibrary				m_dxgi;
		IO::TinySharedMemory			m_sharedCaptureMemery;
		IO::TinySharedMemory			m_textureMemery;
	};
}

