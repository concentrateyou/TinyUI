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
	//////////////////////////////////////////////////////////////////////////
	DX9Capture::DX9Capture()
		:m_d3dFormat(D3DFMT_UNKNOWN),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_patchType(0),
		m_hhk(NULL)
	{

	}
	DX9Capture::~DX9Capture()
	{
		if (m_hhk != NULL)
			UnhookWindowsHookEx(m_hhk);
	}
	DX9Capture& DX9Capture::Instance()
	{
		static DX9Capture instance;
		return instance;
	}
	BOOL DX9Capture::Initialize(HWND hWND)
	{
		if (!BuildEvents())
			return FALSE;
		if (!m_memery.Open(SHAREDCAPTURE_MEMORY) &&
			!m_memery.Create(SHAREDCAPTURE_MEMORY, sizeof(SharedCaptureDATA)))
			return FALSE;
		if (!m_memery.Map(0, 0))
			return FALSE;
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d9.dll"));
		m_d3d9.Reset(szD3DPath);
		if (!m_d3d9.IsValid())
			return FALSE;
		m_d3d10_1.Reset(TEXT("d3d10_1.dll"));
		if (!m_d3d10_1.IsValid())
			return FALSE;
		m_dxgi.Reset(TEXT("dxgi.dll"));
		if (!m_dxgi.IsValid())
			return FALSE;
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)m_d3d9.GetFunctionPointer(TEXT("Direct3DCreate9Ex"));
		if (!d3d9CreateEx)
			return FALSE;
		TinyComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
			return FALSE;
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
			return FALSE;
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX9EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)DX9EndScene))
		{
			return m_dX9EndScene.BeginDetour();
		}
		return FALSE;
	}
	BOOL DX9Capture::BuildEvents()
	{
		DWORD dwProcessID = GetCurrentProcessId();
		if (!m_start)
		{
			string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, dwProcessID);
			if (!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_stop)
		{
			string name = StringPrintf("%s%d", END_CAPTURE_EVENT, dwProcessID);
			if (!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_ready)
		{
			string name = StringPrintf("%s%d", CAPTURE_READY_EVENT, dwProcessID);
			if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		if (!m_exit)
		{
			string name = StringPrintf("%s%d", CAPTURE_EXIT_EVENT, dwProcessID);
			if (!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
				return FALSE;
		}
		return TRUE;
	}
	BOOL DX9Capture::Render(IDirect3DDevice9 *d3d)
	{
		ASSERT(d3d);
		if (m_bCapturing && m_stop.Lock(0))
		{
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_start.Lock(0))
		{
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bTextures)
			{
				m_patchType = GetDX9PatchType(m_d3d9);
				TinyComPtr<IDirect3DSurface9> backBuffer;
				if (SUCCEEDED(d3d->GetRenderTarget(0, &backBuffer)))
				{
					D3DSURFACE_DESC sd;
					::ZeroMemory(&sd, sizeof(sd));
					if (SUCCEEDED(backBuffer->GetDesc(&sd)))
					{
						m_d3dFormat = sd.Format;
						m_dxgiFormat = GetDXGIFormat(sd.Format);
						SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)m_memery.Address();
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
	void DX9Capture::Reset()
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_dX9TextureSurface.Release();
		m_d3d.Release();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
	}
	void DX9Capture::Setup(IDirect3DDevice9 *pThis)
	{
		TinyComPtr<IDirect3DSwapChain9> swapChain;
		if (SUCCEEDED(pThis->GetSwapChain(0, &swapChain)))
		{
			D3DPRESENT_PARAMETERS pp;
			if (SUCCEEDED(swapChain->GetPresentParameters(&pp)))
			{
				SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX9Capture::Instance().m_memery.Address();
				ASSERT(sharedCapture);
				sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
				sharedCapture->Format = pp.BackBufferFormat;
				sharedCapture->Size.cx = pp.BackBufferWidth;
				sharedCapture->Size.cy = pp.BackBufferHeight;
				sharedCapture->HwndCapture = pp.hDeviceWindow;
				if (m_hhk != NULL)
					UnhookWindowsHookEx(m_hhk);
				m_hhk = SetWindowsHookEx(WH_CBT, DX9Capture::CbtFilterHook, NULL, GetCurrentThreadId());
				DX9Capture::Instance().m_dX9PresentEx.Initialize(GetVTable(pThis, (484 / 4)), (FARPROC)DX9PresentEx);
				DX9Capture::Instance().m_dX9PresentEx.BeginDetour();
				DX9Capture::Instance().m_dX9ResetEx.Initialize(GetVTable(pThis, (528 / 4)), (FARPROC)DX9ResetEx);
				DX9Capture::Instance().m_dX9ResetEx.BeginDetour();
				DX9Capture::Instance().m_dX9Present.Initialize(GetVTable(pThis, (68 / 4)), (FARPROC)DX9Present);
				DX9Capture::Instance().m_dX9Present.BeginDetour();
				DX9Capture::Instance().m_dX9Reset.Initialize(GetVTable(pThis, (64 / 4)), (FARPROC)DX9Reset);
				DX9Capture::Instance().m_dX9Reset.BeginDetour();
				DX9Capture::Instance().m_dX9SwapPresent.Initialize(GetVTable(swapChain, (12 / 4)), (FARPROC)DX9SwapPresent);
				DX9Capture::Instance().m_dX9SwapPresent.BeginDetour();
			}
		}
	}
	BOOL DX9Capture::DX9GPUHook(IDirect3DDevice9 *pThis)
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
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_d3d)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d)))
			{
				return FALSE;
			}
		}
		SharedCaptureDATA* sharedCapture = reinterpret_cast<SharedCaptureDATA*>(m_memery.Address());
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
		if (FAILED(m_d3d->CreateTexture2D(&desc, NULL, &d3d10Texture2D)))
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
		LPBYTE patchAddress = (m_patchType != 0) ? GetDX9PatchAddress(m_d3d9, m_patchType) : NULL;
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
		if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
		{
			if (!m_textureMemery.Create(TEXTURE_MEMORY, sizeof(SharedTextureDATA)))
				return FALSE;
		}
		if (!m_textureMemery.Map(0, 0))
			return FALSE;
		if (SharedTextureDATA* sharedTexture = (SharedTextureDATA*)m_textureMemery.Address())
		{
			sharedTexture->TextureHandle = m_hTextureHandle;
		}
		sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
		sharedCapture->bFlip = FALSE;
		m_ready.SetEvent();
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
	LRESULT CALLBACK DX9Capture::CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam)
	{
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(DX9Capture::Instance().m_hhk, code, wParam, lParam);
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX9Capture::Instance().m_memery.Address();
		if (sharedCapture->HwndCapture == reinterpret_cast<HWND>(wParam))
		{
			DX9Capture::Instance().Reset();
			DX9Capture::Instance().m_exit.SetEvent();
		}
		return CallNextHookEx(DX9Capture::Instance().m_hhk, code, wParam, lParam);
	}
}