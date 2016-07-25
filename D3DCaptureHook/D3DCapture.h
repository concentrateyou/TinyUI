#pragma once
#include <d3d9.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include "D3DCommon.h"
#include "D3DDetour.h"
#include "D3DCore.h"
#include "D3DThunk.h"
#include "SharedMemory.h"
#include <string>
using namespace std;

namespace D3D
{
	typedef HRESULT(WINAPI *CREATEDXGIFACTORY1PROC)(REFIID riid, void **ppFactory);

#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define NUM_BUFFERS				3
#define ZERO_ARRAY				{0, 0, 0}
	/// <summary>
	/// D3D9
	/// </summary>
	class D3D9Capture
	{
	public:
		D3D9Capture();
		~D3D9Capture();
		BOOL Initialize(HWND hWND);
		BOOL D3D9DrawStuff(IDirect3DDevice9 *device);
		BOOL D3D9GPUHook(IDirect3DDevice9 *device);
		UINT InitializeSharedMemoryGPUCapture(SharedTextureData **texData);
		static D3D9Capture& Instance();
	private:
		static HRESULT STDMETHODCALLTYPE D3D9EndScene(IDirect3DDevice9 *device);
		static HRESULT STDMETHODCALLTYPE D3D9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		static HRESULT STDMETHODCALLTYPE D3D9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params);
		static HRESULT STDMETHODCALLTYPE D3D9ResetEx(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);
	public:
		CaptureShare				m_d3dCaptureShare;
		D3DFORMAT					m_d3dFormat;
		DXGI_FORMAT					m_dxgiFormat;
		HMODULE						m_hInstance;
		HANDLE						m_sharedHandle;
		BOOL						m_bCapturing;
		BOOL						m_bTextures;
		BOOL						m_bUseSharedTextures;
		DWORD						m_dwCurrentCapture;
		SharedTextureData*			m_sharedTexture;
		IDirect3DDevice9*			m_currentD3DDevice;
		CLock						m_lock;
		CPerformanceTimer			m_timer;
		CComPtr<ID3D10Device1>		m_d3d10Device1;
		CComPtr<ID3D10Resource>		m_d3d10Resource;
		CComPtr<IDirect3DSurface9>	m_d3d9TextureGame;
		CSharedMemory				m_textureMemery;
		INT							m_patchType;
		DWORD						m_sharedMemoryIDCounter;
		D3DDetour					m_d3d9EndScene;
		D3DDetour					m_d3d9Reset;
		D3DDetour					m_d3d9ResetEx;
		D3DDetour					m_d3d9Present;
		D3DDetour					m_d3d9PresentEx;
		D3DDetour					m_d3d9SwapPresent;
		LONGLONG					m_lastTime;
	};
}
