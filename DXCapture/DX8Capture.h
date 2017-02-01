#pragma once
#include "d3d8.h"
#include "DX.h"

namespace DXCapture
{
#define NUM_BUFFERS 3

	typedef IDirect3D8* (WINAPI*D3D8CREATEPROC)(UINT);
	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *device);
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
		void Reset();
		BOOL Setup(IDirect3DDevice8 *pThis);
		BOOL DX8CPUHook(IDirect3DDevice8 *device);
	private:
		void OnMessagePump();
	public:
		IO::TinyTaskBase				m_captureTask;
		LPBYTE							m_textures[2];
		LPVOID							m_currentPointer;
		SharedCaptureDATA				m_captureDATA;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		TinyComPtr<IDirect3DSurface8>	m_surfaces[NUM_BUFFERS];
		BOOL							m_locks[NUM_BUFFERS];
		TinyLock						m_mutexs[NUM_BUFFERS];
		void *volatile					m_currentBits;
		UINT							m_currentTexture;
		TinyEvent						m_copy;
		TinyEvent						m_close;
		TinyDetour						m_dX8EndScene;
		TinyDetour						m_dX8Present;
		TinyDetour						m_dX8Reset;
		HMODULE							m_hD3D8;
		DX&								m_dx;
	};
	SELECTANY extern DX8Capture g_dx8(g_dx);
}

