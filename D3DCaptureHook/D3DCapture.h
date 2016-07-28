#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include "D3DCommon.h"
#include "D3DDetour.h"
#include "D3DCore.h"
#include "SharedMemory.h"
#include <string>
using namespace std;

namespace D3D
{
	typedef HRESULT(WINAPI *CREATEDXGIFACTORY1PROC)(REFIID riid, void **ppFactory);

#define SHAREDCAPTURE			TEXT("Local\\SharedCapture")
#define TEXTURE_MEMORY          TEXT("Local\\TextureMemory")
#define NUM_BUFFERS				3
#define ZERO_ARRAY				{0, 0, 0}
#define BEGIN_CAPTURE_EVENT		TEXT("BeginCapture")
#define END_CAPTURE_EVENT       TEXT("EndCapture")
#define CAPTURE_READY_EVENT     TEXT("CaptureReady")
#define CAPTURETYPE_MEMORY      1
#define CAPTURETYPE_SHAREDTEX   2
	typedef struct tagSharedCapture
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedCapture;//共享的捕获参数
#pragma pack(push, 8)
	typedef struct tagSharedTexture
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTexture;
#pragma pack(pop)
	/// <summary>
	/// D3D9
	/// </summary>
	class D3D9Capture
	{
	public:
		D3D9Capture();
		~D3D9Capture();
		BOOL Initialize(HWND hWND);
		BOOL D3D9Draw(IDirect3DDevice9 *device);
		void D3DReset();
		BOOL D3D9GPUHook(IDirect3DDevice9 *device);
		BOOL InitializeSharedMemoryGPUCapture(SharedTexture **texData);
		static D3D9Capture& Instance();
	private:
		static HRESULT STDMETHODCALLTYPE D3D9EndScene(IDirect3DDevice9 *device);
		static HRESULT STDMETHODCALLTYPE D3D9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
		static HRESULT STDMETHODCALLTYPE D3D9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
		static HRESULT STDMETHODCALLTYPE D3D9Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params);
		static HRESULT STDMETHODCALLTYPE D3D9ResetEx(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData);
	public:
		SharedTexture*			m_pSharedTextureData;
		D3DFORMAT					m_d3dFormat;
		DXGI_FORMAT					m_dxgiFormat;
		CScopedLibrary				m_d3d9;
		CScopedLibrary				m_d3d10_1;
		CScopedLibrary				m_dxgi;
		HANDLE						m_pSharedHandle;
		BOOL						m_bCapturing;
		BOOL						m_bTextures;
		BOOL						m_bUseSharedTextures;
		DWORD						m_dwCurrentCapture;
		IDirect3DDevice9*			m_pGameDevice;
		CLock						m_lock;
		CPerformanceTimer			m_timer;
		CComPtr<ID3D10Device1>		m_d3d10Device1;
		CComPtr<ID3D10Resource>		m_d3d10Resource;
		CComPtr<IDirect3DSurface9>	m_d3d9TextureGame;
		CSharedMemory				m_sharedCapture;
		CSharedMemory				m_textureMemery;
		CD3DDetour					m_d3d9EndScene;
		CD3DDetour					m_d3d9Reset;
		CD3DDetour					m_d3d9ResetEx;
		CD3DDetour					m_d3d9Present;
		CD3DDetour					m_d3d9PresentEx;
		CD3DDetour					m_d3d9SwapPresent;
		CEvent						m_eventBegin;
		CEvent						m_eventEnd;
		CEvent						m_eventReady;
		INT							m_patchType;
		LONGLONG					m_lastTime;
	};
}
