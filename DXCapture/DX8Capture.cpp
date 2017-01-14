#include "stdafx.h"
#include "DX8Capture.h"

namespace DXCapture
{

	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *pThis)
	{
		g_dx8.Setup(pThis);
		g_dx8.m_dX8EndScene.EndDetour();
		HRESULT hRes = pThis->EndScene();
		g_dx8.m_dX8EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		g_dx8.Render(pThis);
		g_dx8.m_dX8Present.EndDetour();
		HRESULT hRes = pThis->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		g_dx8.m_dX8Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* pThis, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		g_dx8.Reset();
		g_dx8.m_dX8Reset.EndDetour();
		HRESULT hRes = pThis->Reset(pPresentationParameters);
		g_dx8.m_dX8Reset.BeginDetour();
		return hRes;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Capture::DX8Capture(DX& dx)
		:m_dx(dx),
		m_currentDevice(NULL)
	{

	}
	DX8Capture::~DX8Capture()
	{

	}
	BOOL DX8Capture::Initialize(HWND hWND)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
		{
			return FALSE;
		}
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d8.dll"));
		m_hD3D8 = GetModuleHandle(szD3DPath);
		if (m_hD3D8 == NULL)
		{
			return FALSE;
		}
		m_d3d10_1.Reset(TEXT("d3d10_1.dll"));
		if (!m_d3d10_1.IsValid())
		{
			return FALSE;
		}
		m_dxgi.Reset(TEXT("dxgi.dll"));
		if (!m_dxgi.IsValid())
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
		TinyComPtr<IDirect3DDevice8>	d3dDevice;
		if (FAILED(hRes = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &d3dDevice)))
		{
			return FALSE;
		}
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX8EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)DX8EndScene))
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
						m_dxgiFormat = GetDXGIFormat(sd.Format);
						m_captureDATA.Format = sd.Format;
						m_captureDATA.Size.cx = sd.Width;
						m_captureDATA.Size.cy = sd.Height;
						m_bTextures = DX8GPUHook(d3d);
					}
				}
			}
			if (m_bTextures)
			{
				TinyComPtr<IDirect3DSurface8> backBuffer;
				if (FAILED(d3d->GetRenderTarget(&backBuffer)))
				{
					if (FAILED(d3d->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
					{
						return FALSE;
					}
				}
				if (!backBuffer || FAILED(d3d->CopyRects(backBuffer, NULL, 0, m_dX8TextureSurface, NULL)))
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	void DX8Capture::Reset(BOOL bRelease)
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_dX8TextureSurface.Release();
		m_d3d10.Release();
		m_dx.m_textureMemery.Unmap();
		m_dx.m_textureMemery.Close();
	}
	void DX8Capture::Setup(IDirect3DDevice8 *pThis)
	{
		TinyComPtr<IDirect3DSurface8> backBuffer;
		if (SUCCEEDED(pThis->GetRenderTarget(&backBuffer)))
		{
			D3DSURFACE_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			if (SUCCEEDED(backBuffer->GetDesc(&desc)))
			{
				m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEX;
				m_captureDATA.Format = desc.Format;
				m_captureDATA.Size.cx = desc.Width;
				m_captureDATA.Size.cy = desc.Height;
				m_captureDATA.HwndCapture = NULL;
				m_dx.SetWindowsHook();
				TinyComPtr<IDirect3D8> d3d8;
				if (SUCCEEDED(pThis->GetDirect3D(&d3d8)))
				{
					m_dX8Present.Initialize(GetVTable(pThis, (56 / 4)), (FARPROC)DX8Present);
					m_dX8Reset.Initialize(GetVTable(pThis, (60 / 4)), (FARPROC)DX8Reset);
					m_dX8Present.BeginDetour();
					m_dX8Reset.BeginDetour();
				}
			}
		}
	}
	BOOL DX8Capture::DX8GPUHook(IDirect3DDevice8 *pThis)
	{
		PFN_D3D10_CREATE_DEVICE1 d3d10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1)m_d3d10_1.GetFunctionPointer(TEXT("D3D10CreateDevice1"));
		if (!d3d10CreateDevice1)
		{
			return FALSE;
		}
		CREATEDXGIFACTORY1PROC createDXGIFactory1 = (CREATEDXGIFACTORY1PROC)m_dxgi.GetFunctionPointer(TEXT("CreateDXGIFactory1"));
		if (!createDXGIFactory1)
		{
			return FALSE;
		}
		TinyComPtr<IDXGIFactory1> factory;
		if (FAILED((*createDXGIFactory1)(__uuidof(IDXGIFactory1), (void**)&factory)))
		{
			return FALSE;
		}
		TinyComPtr<IDXGIAdapter1> adapter;
		if (FAILED(factory->EnumAdapters1(0, &adapter)))
		{
			return FALSE;
		}
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_d3d10)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d10)))
			{
				return FALSE;
			}
		}
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_captureDATA.Size.cx;
		desc.Height = m_captureDATA.Size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D10Texture2D> d3d10Texture2D;
		if (FAILED(m_d3d10->CreateTexture2D(&desc, NULL, &d3d10Texture2D)))
		{
			return FALSE;
		}
		TinyComPtr<IDXGIResource> dxgiResource;
		if (FAILED(d3d10Texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
		{
			return FALSE;
		}
		if (FAILED(dxgiResource->GetSharedHandle(&m_hTextureHandle)))
		{
			return FALSE;
		}
		TinyComPtr<IDirect3DTexture8> d3d8Texture;
		if (FAILED(pThis->CreateTexture(m_captureDATA.Size.cx, m_captureDATA.Size.cy, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_d3dFormat, D3DPOOL_DEFAULT, &d3d8Texture)))
		{
			return FALSE;
		}
		if (FAILED(d3d8Texture->GetSurfaceLevel(0, &m_dX8TextureSurface)))
		{
			return FALSE;
		}
		m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEX;
		m_captureDATA.bFlip = FALSE;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA();
		sharedTexture->TextureHandle = m_hTextureHandle;
		m_dx.m_ready.SetEvent();
		LOG(INFO) << "DX9GPUHook ok\n";
		return TRUE;
	}
}