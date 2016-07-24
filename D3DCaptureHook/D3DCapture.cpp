#include "stdafx.h"
#include "D3DCapture.h"

typedef IDirect3D9* (WINAPI*D3D9CREATEPROC)(UINT);
typedef HRESULT(WINAPI*D3D9CREATEEXPROC)(UINT, IDirect3D9Ex**);

namespace D3D
{
	D3D9Capture::D3D9Capture()
		:m_hInstance(NULL),
		m_lastTime(0),
		m_bD3dEx(FALSE)
	{

	}
	D3D9Capture::~D3D9Capture()
	{

	}
	D3D9Capture& D3D9Capture::Instance()
	{
		static D3D9Capture instance;
		return instance;
	}
	BOOL D3D9Capture::Initialize(HWND hWND)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d9.dll"));
		m_hInstance = GetModuleHandle(szD3DPath);
		if (!m_hInstance)
			return FALSE;
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hInstance, "Direct3DCreate9Ex");
		if (!d3d9CreateEx)
			return FALSE;
		CComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
			return FALSE;
		D3DPRESENT_PARAMETERS pp;
		ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = 1;
		pp.SwapEffect = D3DSWAPEFFECT_FLIP;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.hDeviceWindow = hWND;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		CComPtr<IDirect3DDevice9Ex>	d3dDevice;
		if (FAILED(hRes = d3d9ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES, &pp, NULL, &d3dDevice)))
			return FALSE;
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_d3d9EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)D3D9EndScene))
		{
			m_d3d9EndScene.BeginDetour();
		}
		return TRUE;
	}

	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9EndScene(IDirect3DDevice9 *device)
	{
		D3D9Capture::Instance().m_lock.Acquire();
		D3D9Capture::Instance().m_d3d9EndScene.EndDetour();
		HRESULT hRes = S_OK;
		CComPtr<IDirect3DSwapChain9> swapChain = NULL;
		hRes = device->GetSwapChain(0, &swapChain);
		if (FAILED(hRes))
		{
			//TODO
		}
		D3DPRESENT_PARAMETERS pp;
		hRes = swapChain->GetPresentParameters(&pp);
		if (FAILED(hRes))
		{
			//TODO
		}
		/*D3D9Capture::Instance().m_captureShare.captureType = CAPTURETYPE_SHAREDTEX;
		D3D9Capture::Instance().m_captureShare.format = pp.BackBufferFormat;
		D3D9Capture::Instance().m_captureShare.cx = pp.BackBufferWidth;
		D3D9Capture::Instance().m_captureShare.cy = pp.BackBufferHeight;
		D3D9Capture::Instance().m_captureShare.hwndCapture = pp.hDeviceWindow;
		D3D9Capture::Instance().m_d3d9PresentEx.Initialize(GetVTable(device, (484 / 4)), (FARPROC)D3D9PresentEx);
		D3D9Capture::Instance().m_d3d9ResetEx.Initialize(GetVTable(device, (528 / 4)), (FARPROC)D3D9ResetEx);
		D3D9Capture::Instance().m_d3d9PresentEx.BeginDetour();
		D3D9Capture::Instance().m_d3d9ResetEx.BeginDetour();
		D3D9Capture::Instance().m_d3d9Present.Initialize(GetVTable(device, (68 / 4)), (FARPROC)D3D9Present);
		D3D9Capture::Instance().m_d3d9Present.BeginDetour();
		D3D9Capture::Instance().m_d3d9Reset.Initialize(GetVTable(device, (64 / 4)), (FARPROC)D3D9Reset);
		D3D9Capture::Instance().m_d3d9Reset.BeginDetour();
		D3D9Capture::Instance().m_d3d9SwapPresent.Initialize(GetVTable(swapChain, (12 / 4)), (FARPROC)D3D9SwapPresent);
		D3D9Capture::Instance().m_d3d9SwapPresent.BeginDetour();*/
		hRes = device->EndScene();
		D3D9Capture::Instance().m_d3d9EndScene.BeginDetour();
		D3D9Capture::Instance().m_lock.Release();
		D3D9Capture::Instance().m_lastTime = 0;
		D3D9Capture::Instance().m_timer.BeginTime();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		D3D9Capture::Instance().m_d3d9Present.EndDetour();
		HRESULT hRes = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		D3D9Capture::Instance().m_d3d9Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		D3D9Capture::Instance().m_d3d9PresentEx.EndDetour();
		HRESULT hRes = device->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		D3D9Capture::Instance().m_d3d9PresentEx.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		D3D9Capture::Instance().m_d3d9SwapPresent.EndDetour();
		HRESULT hRes = swap->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		D3D9Capture::Instance().m_d3d9SwapPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params)
	{
		D3D9Capture::Instance().m_d3d9Reset.EndDetour();
		HRESULT hRes = device->Reset(params);
		D3D9Capture::Instance().m_d3d9Reset.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9ResetEx(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData)
	{
		D3D9Capture::Instance().m_d3d9ResetEx.EndDetour();
		HRESULT hRes = device->ResetEx(params, fullscreenData);
		D3D9Capture::Instance().m_d3d9ResetEx.BeginDetour();
		return hRes;
	}
}