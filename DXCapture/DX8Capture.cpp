#include "stdafx.h"
#include "DX8Capture.h"

namespace DXCapture
{

	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *pThis)
	{
		if (g_dx8.m_pThis == NULL)
		{
			//g_dx8.m_pThis = g_dx8.Setup(pThis) ? pThis : NULL;
		}
		g_dx8.m_dX8EndScene.EndDetour();
		HRESULT hRes = pThis->EndScene();
		g_dx8.m_dX8EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		if (g_dx8.m_pThis == pThis)
		{
			//g_dx8.Render(pThis);
		}
		g_dx8.m_dX8Present.EndDetour();
		HRESULT hRes = pThis->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		g_dx8.m_dX8Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* pThis, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (g_dx8.m_pThis == pThis)
		{
			//g_dx8.Reset();
		}
		g_dx8.m_dX8Reset.EndDetour();
		HRESULT hRes = pThis->Reset(pPresentationParameters);
		g_dx8.m_dX8Reset.BeginDetour();
		return hRes;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Capture::DX8Capture(DX& dx)
		:m_dx(dx),
		m_pThis(NULL)
	{

	}
	DX8Capture::~DX8Capture()
	{

	}
	BOOL DX8Capture::Initialize(HWND hWND)
	{
		HRESULT hRes = S_OK;
		CHAR szPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szPath)))
		{
			return FALSE;
		}
		strcat_s(szPath, MAX_PATH, TEXT("\\d3d8.dll"));
		m_hD3D8 = GetModuleHandle(szPath);
		if (m_hD3D8 == NULL)
		{
			return FALSE;
		}
		D3D8CREATEPROC d3d8Create = (D3D8CREATEPROC)GetProcAddress(m_hD3D8, TEXT("Direct3DCreate8"));
		if (!d3d8Create)
		{
			return FALSE;
		}
		TinyComPtr<IDirect3D8> d3d8(d3d8Create(D3D_SDK_VERSION));
		D3DPRESENT_PARAMETERS pp;
		::ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = 1;
		pp.SwapEffect = D3DSWAPEFFECT_FLIP;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.hDeviceWindow = hWND;
		TinyComPtr<IDirect3DDevice8> d3dDevice;
		if (FAILED(hRes = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &d3dDevice)))
		{
			return FALSE;
		}
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX8EndScene.Initialize((FARPROC)*(vtable + 35), (FARPROC)DX8EndScene))
		{
			if (m_dX8EndScene.BeginDetour())
			{
				LOG(INFO) << "m_dX8EndScene.BeginDetour()\n";
				return TRUE;
			}
		}
		return TRUE;
	}
	BOOL DX8Capture::Render(IDirect3DDevice8 *d3d)
	{
		ASSERT(d3d);
		if (m_bCapturing && m_dx.m_stop.Lock(0))
		{
			LOG(INFO) << "DX8Capture m_stop\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.Lock(0))
		{
			LOG(INFO) << "DX8Capture m_start\n";
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bTextures)
			{
				TinyComPtr<IDirect3DSurface8> backBuffer;
				if (SUCCEEDED(d3d->GetRenderTarget(&backBuffer)))
				{
					D3DSURFACE_DESC sd;
					::ZeroMemory(&sd, sizeof(sd));
					if (SUCCEEDED(backBuffer->GetDesc(&sd)))
					{
						m_d3dFormat = sd.Format;
						m_captureDATA.Format = sd.Format;
						m_captureDATA.Size.cx = sd.Width;
						m_captureDATA.Size.cy = sd.Height;
						m_bTextures = DX8CPUHook(d3d);
					}
				}
			}
			if (m_bTextures)
			{
				TinyComPtr<IDirect3DSurface8> backBuffer;
				if (FAILED(d3d->CreateImageSurface(m_captureDATA.Size.cx, m_captureDATA.Size.cy, (D3DFORMAT)m_captureDATA.Format, &backBuffer)))
				{
					return FALSE;
				}
				D3DLOCKED_RECT lr;
				if (FAILED(backBuffer->LockRect(&lr, NULL, D3DLOCK_READONLY)))
				{
					return FALSE;
				}
				backBuffer->UnlockRect();
			}
		}
		return TRUE;
	}
	void DX8Capture::Reset()
	{
		m_bTextures = FALSE;
		m_dx.m_textureMemery.Unmap();
		m_dx.m_textureMemery.Close();
	}
	BOOL DX8Capture::Setup(IDirect3DDevice8 *pThis)
	{
		TinyComPtr<IDirect3DSurface8> backBuffer;
		if (SUCCEEDED(pThis->GetRenderTarget(&backBuffer)))
		{
			D3DSURFACE_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			if (SUCCEEDED(backBuffer->GetDesc(&desc)))
			{
				m_captureDATA.CaptureType = CAPTURETYPE_MEMORY;
				m_captureDATA.Format = desc.Format;
				m_captureDATA.Size.cx = desc.Width;
				m_captureDATA.Size.cy = desc.Height;
				m_captureDATA.HwndCapture = NULL;
				m_captureDATA.Pitch = 4 * desc.Height;
				m_dx.SetWindowsHook();
				TinyComPtr<IDirect3D8> d3d8;
				if (SUCCEEDED(pThis->GetDirect3D(&d3d8)))
				{
					m_dX8Present.Initialize(GetVTable(pThis, 15), (FARPROC)DX8Present);
					m_dX8Reset.Initialize(GetVTable(pThis, 14), (FARPROC)DX8Reset);
					m_dX8Present.BeginDetour();
					m_dX8Reset.BeginDetour();
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	BOOL DX8Capture::DX8CPUHook(IDirect3DDevice8 *pThis)
	{
		m_captureDATA.CaptureType = CAPTURETYPE_MEMORY;
		m_captureDATA.bFlip = FALSE;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));

		m_dx.m_ready.SetEvent();
		LOG(INFO) << "DX8CPUHook ok\n";
		return TRUE;
	}
}