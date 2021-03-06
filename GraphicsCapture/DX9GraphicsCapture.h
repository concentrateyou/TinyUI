#pragma once
#include "DX.h"
#include <d3d9.h>

namespace GraphicsCapture
{
	typedef IDirect3D9* (WINAPI*D3D9CREATEPROC)(UINT);
	typedef HRESULT(WINAPI*D3D9CREATEEXPROC)(UINT, IDirect3D9Ex**);

	HRESULT STDMETHODCALLTYPE DX9EndScene(IDirect3DDevice9 *pThis);
	HRESULT STDMETHODCALLTYPE DX9Present(IDirect3DDevice9 *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	HRESULT STDMETHODCALLTYPE DX9PresentEx(IDirect3DDevice9Ex *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE DX9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE DX9Reset(IDirect3DDevice9 *pThis, D3DPRESENT_PARAMETERS *params);
	HRESULT STDMETHODCALLTYPE DX9ResetEx(IDirect3DDevice9Ex *pThis, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);

	DXGI_FORMAT GetDXGIFormat(D3DFORMAT format);

	class DX9CaptureDATA
	{
		DISALLOW_COPY_AND_ASSIGN(DX9CaptureDATA)
	public:
		DX9CaptureDATA();
		~DX9CaptureDATA();
	public:
		BOOL				Create(IDirect3DDevice9* pThis, INT cx, INT cy, D3DFORMAT format);
		void				Destory();
		BOOL				Enter();
		void				Leave();
		BOOL				TestF(INT32 val) const;
		void				SetF(INT32 val);
		void				CrlF(INT32 val);
		UINT32				GetPitch() const;
		IDirect3DQuery9*	GetQuery();
		IDirect3DSurface9*	GetCopy2D();
		IDirect3DSurface9*	GetTexture2D();
	private:
		volatile	INT32				m_iFlag;
		UINT32							m_linesize;
		TinySize						m_size;
		TinyLock						m_lock;
		TinyComPtr<IDirect3DQuery9>		m_query;
		TinyComPtr<IDirect3DSurface9>	m_copy2D;
		TinyComPtr<IDirect3DSurface9>	m_texture2D;
	};

	/// <summary>
	/// XX9�������ݲ���
	/// </summary>
	class DX9GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX9GraphicsCapture)
	public:
		DX9GraphicsCapture(DX& dx);
		~DX9GraphicsCapture();
		BOOL Initialize(HWND hWND);
		BOOL Draw(IDirect3DDevice9 *device);
		void Reset();
		BOOL Setup(IDirect3DDevice9 *pThis);
		BOOL DX9GPUHook(IDirect3DDevice9 *device);
		BOOL DX9CPUHook(IDirect3DDevice9 *device);
	public:
		BOOL hookable();
	private:
		DX9CaptureDATA* GetDX9CaptureDATA(LPVOID& data);
		void OnMessagePump();
		void QueryCopy(IDirect3DDevice9 *device);
	public:
		DX&								m_dx;
		BOOL							m_bD3D9EX;
		BOOL							m_bGPU;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		volatile INT32					m_currentCopy;
		TLS								m_tls;
		INT32							m_patchType;
		HANDLE							m_handle;
		HMODULE							m_hD3D9;
		LPVOID							m_currentPointer;
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		TinyWorker						m_captureTask;
		DX9CaptureDATA*					m_captures[NUM_BUFFERS];
		TinyComPtr<ID3D11Device>		m_device;
		TinyComPtr<ID3D11Texture2D>		m_texture2D;
		TinyComPtr<IDirect3DSurface9>	m_copy2D;
		TinyLock						m_lock;
		TinyEvent						m_copy;
		TinyEvent						m_stop;
		TinyDetour						m_dX9EndScene;
		TinyDetour						m_dX9Reset;
		TinyDetour						m_dX9ResetEx;
		TinyDetour						m_dX9Present;
		TinyDetour						m_dX9PresentEx;
		TinyDetour						m_dX9SwapPresent;
		TinyScopedLibrary				m_d3d11;
		TinyScopedLibrary				m_dxgi;
	};
	SELECTANY extern DX9GraphicsCapture g_dx9(g_dx);
}

