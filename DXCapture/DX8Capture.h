#pragma once
#include "d3d8.h"
#include "d3d8caps.h"
#include "d3d8types.h"
#include "DX.h"

namespace DXCapture
{
	typedef IDirect3D8* (WINAPI*D3D8CREATEPROC)(UINT);
	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *pThis);
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	/// <summary>
	/// XX8纹理数据捕获
	/// </summary>
	class DX8Capture
	{
	public:
		DX8Capture(DX& dx);
		~DX8Capture();
		BOOL Initialize(HWND hWND);
		BOOL Render(IDirect3DDevice8 *device);
		void Reset(BOOL bRelease = TRUE);
		void Setup(IDirect3DDevice8 *pThis);
		BOOL DX8GPUHook(IDirect3DDevice8 *device);
	public:
		LPVOID							m_currentDevice;
		SharedCaptureDATA				m_captureDATA;
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bDetour;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		TinyDetour						m_dX8EndScene;
		TinyDetour						m_dX8Present;
		TinyDetour						m_dX8Reset;
		HMODULE							m_hD3D8;
		TinyComPtr<ID3D10Device1>		m_d3d10;
		TinyComPtr<IDirect3DSurface8>	m_dX8TextureSurface;
		TinyScopedLibrary				m_d3d10_1;
		TinyScopedLibrary				m_dxgi;
		DX&								m_dx;
	};
	SELECTANY extern DX8Capture g_dx8(g_dx);
}

