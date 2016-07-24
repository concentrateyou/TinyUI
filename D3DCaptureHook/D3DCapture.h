#pragma once
#include <d3d9.h>
#include <D3D10_1.h>
#include "D3DCommon.h"
#include "D3DDetour.h"
#include "D3DSemaphore.h"
#include "D3DThunk.h"
#include <string>
using namespace std;

namespace D3D
{
	/// <summary>
	/// D3D9
	/// </summary>
	class D3D9Capture
	{
	public:
		D3D9Capture();
		~D3D9Capture();
		BOOL Initialize(HWND hWND);
		static D3D9Capture& Instance();
	private:
		static HRESULT STDMETHODCALLTYPE D3D9EndScene(IDirect3DDevice9 *device);
		static HRESULT STDMETHODCALLTYPE D3D9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		static HRESULT STDMETHODCALLTYPE D3D9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params);
		static HRESULT STDMETHODCALLTYPE D3D9ResetEx(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);
	public:
		BOOL						m_bD3dEx;
		CLock						m_lock;
		CaptureShare				m_captureShare;
		HMODULE						m_hInstance;
		D3DDetour					m_d3d9EndScene;
		D3DDetour					m_d3d9Reset;
		D3DDetour					m_d3d9ResetEx;
		D3DDetour					m_d3d9Present;
		D3DDetour					m_d3d9PresentEx;
		D3DDetour					m_d3d9SwapPresent;
		LONGLONG					m_lastTime;
		CPerformanceTimer			m_timer;
	};
}
