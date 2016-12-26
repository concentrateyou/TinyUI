#include "stdafx.h"
#include "DX9Capture.h"

namespace DXCapture
{
	struct PatchInfo
	{
		size_t patchSize;
		const BYTE *patchData;
	};
#define NewPatch(x) {sizeof(x), (x)}
#define NUM_KNOWN_PATCHES 14
#define PATCH_COMPARE_SIZE 12
	ULONG patch_offsets[NUM_KNOWN_PATCHES] = { 0x79AA6, 0x79C9E, 0x79D96, 0x7F9BD, 0x8A3F4, 0x8B15F, 0x8B19F, 0x8B83F, 0x8E9F7, 0x8F00F, 0x8FBB1, 0x90264, 0x90C57, 0x166A08 };
	BYTE patch_compare[NUM_KNOWN_PATCHES][PATCH_COMPARE_SIZE] =
	{
		{ 0x8b, 0x89, 0xe8, 0x29, 0x00, 0x00, 0x39, 0xb9, 0x80, 0x4b, 0x00, 0x00 },  //win7   - 6.1.7601.16562
		{ 0x8b, 0x89, 0xe8, 0x29, 0x00, 0x00, 0x39, 0xb9, 0x80, 0x4b, 0x00, 0x00 },  //win7   - 6.1.7600.16385
		{ 0x8b, 0x89, 0xe8, 0x29, 0x00, 0x00, 0x39, 0xb9, 0x80, 0x4b, 0x00, 0x00 },  //win7   - 6.1.7601.17514
		{ 0x8b, 0x80, 0xe8, 0x29, 0x00, 0x00, 0x39, 0xb0, 0x40, 0x4c, 0x00, 0x00 },  //win8.1 - 6.3.9431.00000
		{ 0x80, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0x40, 0x4c, 0x00, 0x00, 0x00 },  //win8.1 - 6.3.9600.16404
		{ 0x81, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0xa0, 0x4c, 0x00, 0x00, 0x00 },  //win10  - 10.0.10240.16384
		{ 0x81, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0xa0, 0x4c, 0x00, 0x00, 0x00 },  //win10  - 10.0.10162.0
		{ 0x81, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0xa0, 0x4c, 0x00, 0x00, 0x00 },  //win10  - 10.0.10240.16412
		{ 0x80, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0x40, 0x4c, 0x00, 0x00, 0x00 },  //win8.1 - 6.3.9600.17095
		{ 0x80, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0x40, 0x4c, 0x00, 0x00, 0x00 },  //win8.1 - 6.3.9600.17085
		{ 0x80, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0x40, 0x4c, 0x00, 0x00, 0x00 },  //win8.1 - 6.3.9600.16384
		{ 0x87, 0xe8, 0x29, 0x00, 0x00, 0x83, 0xb8, 0x40, 0x4c, 0x00, 0x00, 0x00 },  //win8.1 - 6.3.9600.17415
		{ 0x81, 0x18, 0x2a, 0x00, 0x00, 0x83, 0xb8, 0xa0, 0x4c, 0x00, 0x00, 0x00 },  //win10  - 10.0.10586.0
		{ 0x8b, 0x80, 0xe8, 0x29, 0x00, 0x00, 0x39, 0x90, 0xb0, 0x4b, 0x00, 0x00 },  //win8   - 6.2.9200.16384
	};
	static const BYTE forceJump[] = { 0xEB };
	static const BYTE ignoreJump[] = { 0x90, 0x90 };
	PatchInfo patch[NUM_KNOWN_PATCHES] =
	{
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(ignoreJump),
		NewPatch(ignoreJump),
		NewPatch(ignoreJump),
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(forceJump),
		NewPatch(ignoreJump),
		NewPatch(ignoreJump),
		NewPatch(forceJump),
	};
	BOOL CompareMemory(const LPVOID lpVal1, const LPVOID lpVal2, UINT nBytes)
	{
		__try
		{
			return memcmp(lpVal1, lpVal2, nBytes) == 0;
		}
		__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			return FALSE;
		}
		return FALSE;
	}
	DXGI_FORMAT GetDXGIFormat(D3DFORMAT format)
	{
		switch (format)
		{
		case D3DFMT_A2B10G10R10:    return DXGI_FORMAT_R10G10B10A2_UNORM;
		case D3DFMT_A8R8G8B8:       return DXGI_FORMAT_B8G8R8A8_UNORM;
		case D3DFMT_X8R8G8B8:       return DXGI_FORMAT_B8G8R8X8_UNORM;
		case D3DFMT_A1R5G5B5:       return DXGI_FORMAT_B5G5R5A1_UNORM;
		case D3DFMT_R5G6B5:         return DXGI_FORMAT_B5G6R5_UNORM;
		}

		return DXGI_FORMAT_UNKNOWN;
	}
	INT GetDX9PatchType(HMODULE hModule)
	{
		LPBYTE lpBaseAddress = (LPBYTE)hModule;
		for (int i = 0; i < NUM_KNOWN_PATCHES; i++)
		{
			if (CompareMemory(lpBaseAddress + patch_offsets[i], patch_compare[i], PATCH_COMPARE_SIZE))
			{
				return i + 1;
			}
		}

		return 0;
	}
	LPBYTE GetDX9PatchAddress(HMODULE hModule, INT patchType)
	{
		if (patchType)
		{
			LPBYTE lpBaseAddress = (LPBYTE)hModule;
			return lpBaseAddress + patch_offsets[patchType - 1] + PATCH_COMPARE_SIZE;
		}
		return NULL;
	}
#ifdef _WIN64
#define CMP_SIZE 21
	static const BYTE mask[CMP_SIZE] = { 0xF8, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00 };
	static const BYTE mask_cmp[CMP_SIZE] = { 0x48, 0x8B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x75, 0x00, 0x40, 0xB8, 0x00, 0x00, 0x00, 0x00 };
#else

#define CMP_SIZE 19
	static const BYTE mask[CMP_SIZE] = { 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 };
	static const BYTE mask_cmp[CMP_SIZE] = { 0x8B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x75, 0x00, 0x68, 0x00, 0x00, 0x00, 0x00 };
#endif
	static inline BOOL PatternMatches(BYTE *byte)
	{
		for (size_t i = 0; i < CMP_SIZE; i++)
		{
			if ((byte[i] & mask[i]) != mask_cmp[i])
				return FALSE;
		}
		return TRUE;
	}
#define MAX_FUNC_SCAN_BYTES 200
	//////////////////////////////////////////////////////////////////////////
	DX9Capture::DX9Capture()
		:m_d3dFormat(D3DFMT_UNKNOWN),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_patchType(0),
		m_hD3D9(NULL)
	{

	}
	DX9Capture::~DX9Capture()
	{

	}
	DX9Capture& DX9Capture::Instance()
	{
		static DX9Capture instance;
		return instance;
	}
	BOOL DX9Capture::Initialize(HWND hWND)
	{
		if (!DXGlobal::Instance().BuildEvents())
		{
			return FALSE;
		}
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
		{
			return FALSE;
		}
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d9.dll"));
		m_hD3D9 = GetModuleHandle(szD3DPath);
		if (m_hD3D9 == NULL)
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
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hD3D9, TEXT("Direct3DCreate9Ex"));
		if (!d3d9CreateEx)
		{
			return FALSE;
		}
		TinyComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
		{
			return FALSE;
		}
		D3DPRESENT_PARAMETERS pp;
		::ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = 1;
		pp.SwapEffect = D3DSWAPEFFECT_FLIP;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.hDeviceWindow = hWND;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		TinyComPtr<IDirect3DDevice9Ex>	d3dDevice;
		if (FAILED(hRes = d3d9ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES, &pp, NULL, &d3dDevice)))
		{
			return FALSE;
		}
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX9EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)DX9EndScene))
		{
			return m_dX9EndScene.BeginDetour();
		}
		return FALSE;
	}
	BOOL DX9Capture::Render(IDirect3DDevice9 *d3d)
	{
		ASSERT(d3d && m_hD3D9);
		if (m_bCapturing && DXGlobal::Instance().m_stop && DXGlobal::Instance().m_stop.Lock(0))
		{
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && DXGlobal::Instance().m_start && DXGlobal::Instance().m_start.Lock(0))
		{
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bTextures)
			{
				m_patchType = GetDX9PatchType(m_hD3D9);
				TinyComPtr<IDirect3DSurface9> backBuffer;
				if (SUCCEEDED(d3d->GetRenderTarget(0, &backBuffer)))
				{
					D3DSURFACE_DESC sd;
					::ZeroMemory(&sd, sizeof(sd));
					if (SUCCEEDED(backBuffer->GetDesc(&sd)))
					{
						m_d3dFormat = sd.Format;
						m_dxgiFormat = GetDXGIFormat(sd.Format);
						SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
						ASSERT(sharedCapture);
						sharedCapture->Format = sd.Format;
						sharedCapture->Size.cx = sd.Width;
						sharedCapture->Size.cy = sd.Height;
						m_bTextures = DX9GPUHook(d3d);
					}
				}
			}
			if (m_bTextures)
			{
				TinyComPtr<IDirect3DSurface9> backBuffer;
				if (FAILED(d3d->GetRenderTarget(0, &backBuffer)))
				{
					if (FAILED(d3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
					{
						return FALSE;
					}
				}
				if (!backBuffer || FAILED(d3d->StretchRect(backBuffer, NULL, m_dX9TextureSurface, NULL, D3DTEXF_NONE)))
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	void DX9Capture::Reset(BOOL bRelease)
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		if (bRelease)
		{
			m_dX9TextureSurface.Release();
			m_d3d10.Release();
		}
		else
		{
			m_dX9TextureSurface.Detach();
			m_d3d10.Detach();
		}
	}
	void DX9Capture::Setup(IDirect3DDevice9 *pThis)
	{
		TinyComPtr<IDirect3DSwapChain9> swapChain;
		if (SUCCEEDED(pThis->GetSwapChain(0, &swapChain)))
		{
			D3DPRESENT_PARAMETERS pp;
			if (SUCCEEDED(swapChain->GetPresentParameters(&pp)))
			{
				SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
				ASSERT(sharedCapture);
				sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
				sharedCapture->Format = pp.BackBufferFormat;
				sharedCapture->Size.cx = pp.BackBufferWidth;
				sharedCapture->Size.cy = pp.BackBufferHeight;
				sharedCapture->HwndCapture = pp.hDeviceWindow;
				DXGlobal::Instance().Hook();
				TinyComPtr<IDirect3D9> d3d9;
				if (SUCCEEDED(pThis->GetDirect3D(&d3d9)))
				{
					TinyComPtr<IDirect3D9Ex> d3d9ex;
					BOOL bRes = SUCCEEDED(d3d9->QueryInterface(__uuidof(IDirect3D9Ex), (void**)&d3d9ex));
					DX9Capture::Instance().m_dX9Present.Initialize(GetVTable(pThis, (68 / 4)), (FARPROC)DX9Present);
					DX9Capture::Instance().m_dX9Reset.Initialize(GetVTable(pThis, (64 / 4)), (FARPROC)DX9Reset);
					DX9Capture::Instance().m_dX9SwapPresent.Initialize(GetVTable(swapChain, (12 / 4)), (FARPROC)DX9SwapPresent);
					if (bRes)
					{
						DX9Capture::Instance().m_dX9PresentEx.BeginDetour();
						DX9Capture::Instance().m_dX9ResetEx.BeginDetour();
					}
					DX9Capture::Instance().m_dX9Present.BeginDetour();
					DX9Capture::Instance().m_dX9Reset.BeginDetour();
					DX9Capture::Instance().m_dX9SwapPresent.BeginDetour();
					if (bRes)
					{
						DX9Capture::Instance().m_dX9PresentEx.Initialize(GetVTable(pThis, (484 / 4)), (FARPROC)DX9PresentEx);
						DX9Capture::Instance().m_dX9ResetEx.Initialize(GetVTable(pThis, (528 / 4)), (FARPROC)DX9ResetEx);
					}
				}
			}
		}
	}
	BOOL DX9Capture::DX9GPUHook(IDirect3DDevice9 *pThis)
	{
		ASSERT(m_hD3D9);
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
		SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = sharedCapture->Size.cx;
		desc.Height = sharedCapture->Size.cy;
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
		LPBYTE patchAddress = (m_patchType != 0) ? GetDX9PatchAddress(m_hD3D9, m_patchType) : NULL;
		DWORD dwOldProtect;
		size_t patchSize;
		TinyScopedArray<BYTE> patchData;
		if (patchAddress)
		{
			patchSize = patch[m_patchType - 1].patchSize;
			patchData.Reset(new BYTE[patchSize]);
			if (!VirtualProtect(patchAddress, patchSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			{
				return FALSE;
			}
			memcpy(patchData.Ptr(), patchAddress, patchSize);
			memcpy(patchAddress, patch[m_patchType - 1].patchData, patchSize);
		}
		TinyComPtr<IDirect3DTexture9> d3d9Texture;
		if (FAILED(pThis->CreateTexture(sharedCapture->Size.cx, sharedCapture->Size.cy, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_d3dFormat, D3DPOOL_DEFAULT, &d3d9Texture, &m_hTextureHandle)))
		{
			return FALSE;
		}
		if (patchAddress)
		{
			memcpy(patchAddress, patchData.Ptr(), patchSize);
			VirtualProtect(patchAddress, patchSize, dwOldProtect, &dwOldProtect);
		}
		if (FAILED(d3d9Texture->GetSurfaceLevel(0, &m_dX9TextureSurface)))
		{
			return FALSE;
		}
		if (SharedTextureDATA* sharedTexture = DXGlobal::Instance().GetSharedTextureDATA())
		{
			sharedTexture->TextureHandle = m_hTextureHandle;
		}
		sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
		sharedCapture->bFlip = FALSE;
		if (DXGlobal::Instance().m_ready)
		{
			DXGlobal::Instance().m_ready.SetEvent();
		}
		return TRUE;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9EndScene(IDirect3DDevice9 *pThis)
	{
		DX9Capture::Instance().m_dX9EndScene.EndDetour();
		HRESULT hRes = S_OK;
		DX9Capture::Instance().m_lock.Lock();
		if (!DX9Capture::Instance().m_bDetour)
		{
			DX9Capture::Instance().m_bDetour = TRUE;
			DX9Capture::Instance().Setup(pThis);
		}
		DX9Capture::Instance().m_lock.Unlock();
		hRes = pThis->EndScene();
		DX9Capture::Instance().m_dX9EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9Present(IDirect3DDevice9 *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		DX9Capture::Instance().m_dX9Present.EndDetour();
		DX9Capture::Instance().Render(pThis);
		HRESULT hRes = pThis->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		DX9Capture::Instance().m_dX9Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9PresentEx(IDirect3DDevice9Ex *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		DX9Capture::Instance().m_dX9PresentEx.EndDetour();
		DX9Capture::Instance().Render(pThis);
		HRESULT hRes = pThis->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		DX9Capture::Instance().m_dX9PresentEx.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9SwapPresent(IDirect3DSwapChain9 *pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		DX9Capture::Instance().m_dX9SwapPresent.EndDetour();
		TinyComPtr<IDirect3DDevice9> device;
		if (SUCCEEDED(pThis->GetDevice(&device)))
		{
			DX9Capture::Instance().Render(device);
		}
		HRESULT hRes = pThis->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		DX9Capture::Instance().m_dX9SwapPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9Reset(IDirect3DDevice9 *pThis, D3DPRESENT_PARAMETERS *params)
	{
		DX9Capture::Instance().m_dX9Reset.EndDetour();
		DX9Capture::Instance().Reset();
		HRESULT hRes = pThis->Reset(params);
		DX9Capture::Instance().m_dX9Reset.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Capture::DX9ResetEx(IDirect3DDevice9Ex *pThis, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData)
	{
		DX9Capture::Instance().m_dX9ResetEx.EndDetour();
		DX9Capture::Instance().Reset();
		HRESULT hRes = pThis->ResetEx(params, fullscreenData);
		DX9Capture::Instance().m_dX9ResetEx.BeginDetour();
		return hRes;
	}
	/*BOOL DX9Capture::SaveBitmap(LPCSTR pzFile)
	{
	if (SharedTextureDATA* sharedTexture = (SharedTextureDATA*)m_textureMemery.Address())
	{
	if (sharedTexture->TextureHandle)
	{
	TinyComPtr<ID3D10Resource> resource;
	HRESULT hRes = m_d3d10->OpenSharedResource(sharedTexture->TextureHandle, __uuidof(ID3D10Resource), (void**)&resource);
	if (FAILED(hRes))
	return FALSE;
	TinyComPtr<ID3D10Texture2D> texture2D;
	if (FAILED(hRes = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
	return FALSE;
	D3D10_TEXTURE2D_DESC desc;
	texture2D->GetDesc(&desc);

	D3D10_TEXTURE2D_DESC desc1 = { 0 };
	ZeroMemory(&desc1, sizeof(D3D10_TEXTURE2D_DESC));
	desc1.Width = desc.Width;
	desc1.Height = desc.Height;
	desc1.MipLevels = 1;
	desc1.ArraySize = 1;
	desc1.Format = desc.Format;
	desc1.MiscFlags = 0;
	desc1.SampleDesc.Count = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.Usage = D3D10_USAGE_STAGING;
	desc1.CPUAccessFlags = D3D10_CPU_ACCESS_READ;

	TinyComPtr<ID3D10Texture2D> d3d10Texture2D;
	hRes = m_d3d10->CreateTexture2D(&desc1, NULL, &d3d10Texture2D);
	if (FAILED(hRes))
	{
	return FALSE;
	}
	m_d3d10->CopyResource(d3d10Texture2D, resource);

	D3D10_MAPPED_TEXTURE2D  mapTexture2D;
	hRes = d3d10Texture2D->Map(0, D3D10_MAP_READ, 0, &mapTexture2D);
	if (FAILED(hRes))
	return FALSE;
	INT linesize = ((((24 * desc.Width) + 31) / 32) * 4);
	DWORD dwSize = desc.Width*desc.Height * 4;
	BITMAPINFOHEADER bi = { 0 };
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biWidth = desc.Width;
	bi.biHeight = desc.Height;
	bi.biPlanes = 1;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	BITMAPFILEHEADER  bmfHeader = { 0 };
	DWORD dwSizeofDIB = dwSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	HANDLE hFile = CreateFile(pzFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)mapTexture2D.pData, dwSize, &dwBytesWritten, NULL);
	CloseHandle(hFile);
	texture2D->Unmap(0);
	}
	}
	return TRUE;
	}*/
}