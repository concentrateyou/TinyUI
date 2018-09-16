#include "stdafx.h"
#include "DX9GraphicsCapture.h"

namespace GraphicsCapture
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
#if defined(_WIN64)
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
	DXGI_FORMAT GetDXGIFormat9(D3DFORMAT format)
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
#define MAX_FUNC_SCAN_BYTES 200
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE DX9EndScene(IDirect3DDevice9 *device)
	{
		g_dx9.m_dX9EndScene.EndDetour();
		if (g_dx9.m_currentPointer == NULL)
		{
			g_dx9.m_currentPointer = g_dx9.Setup(device) ? device : NULL;
		}
		HRESULT hRes = device->EndScene();
		g_dx9.m_dX9EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		g_dx9.m_dX9Present.EndDetour();
		if (g_dx9.m_currentPointer == device)
		{
			g_dx9.Render(device);
		}
		HRESULT hRes = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		g_dx9.m_dX9Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		g_dx9.m_dX9PresentEx.EndDetour();
		if (g_dx9.m_currentPointer == device)
		{
			g_dx9.Render(device);
		}
		HRESULT hRes = device->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		g_dx9.m_dX9PresentEx.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9SwapPresent(IDirect3DSwapChain9 *swap, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		g_dx9.m_dX9SwapPresent.EndDetour();
		TinyComPtr<IDirect3DDevice9> device = NULL;
		if (SUCCEEDED(swap->GetDevice(&device)))
		{
			if (g_dx9.m_currentPointer == device)
			{
				g_dx9.Render(device);
			}
		}
		HRESULT hRes = swap->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		g_dx9.m_dX9SwapPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9Reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params)
	{
		g_dx9.m_dX9Reset.EndDetour();
		if (g_dx9.m_currentPointer == device)
		{
			g_dx9.Reset();
		}
		HRESULT hRes = device->Reset(params);
		g_dx9.m_dX9Reset.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX9ResetEx(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *params, D3DDISPLAYMODEEX *fullscreenData)
	{
		g_dx9.m_dX9ResetEx.EndDetour();
		if (g_dx9.m_currentPointer == device)
		{
			g_dx9.Reset();
		}
		HRESULT hRes = device->ResetEx(params, fullscreenData);
		g_dx9.m_dX9ResetEx.BeginDetour();
		return hRes;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9CaptureDATA::DX9CaptureDATA()
		:m_bCopying(FALSE),
		m_bits(NULL),
		m_bIssue(FALSE),
		m_pitch(0)
	{

	}
	DX9CaptureDATA::~DX9CaptureDATA()
	{
		Destory();
	}
	BOOL DX9CaptureDATA::Create(IDirect3DDevice9* pThis, INT cx, INT cy, D3DFORMAT format)
	{
		Destory();
		HRESULT hRes = S_OK;
		hRes = pThis->CreateOffscreenPlainSurface(cx, cy, (D3DFORMAT)format, D3DPOOL_SYSTEMMEM, &m_copySurface, NULL);
		if (FAILED(hRes))
		{
			LOG(INFO) << "[DX9CaptureDATA] CreateOffscreenPlainSurface FAIL\n";
			return FALSE;
		}
		D3DLOCKED_RECT lockedRect;
		hRes = m_copySurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
		if (FAILED(hRes))
		{
			LOG(INFO) << "[DX9CaptureDATA] LockRect FAIL\n";
			return FALSE;
		}
		m_pitch = lockedRect.Pitch;
		m_copySurface->UnlockRect();
		hRes = pThis->CreateRenderTarget(cx, cy, (D3DFORMAT)format, D3DMULTISAMPLE_NONE, 0, FALSE, &m_renderTarget, NULL);
		if (FAILED(hRes))
		{
			LOG(INFO) << "[DX9CaptureDATA] CreateRenderTarget FAIL\n";
			return FALSE;
		}
		hRes = pThis->CreateQuery(D3DQUERYTYPE_EVENT, &m_query);
		if (FAILED(hRes))
		{
			LOG(INFO) << "[DX9CaptureDATA] CreateQuery FAIL\n";
			return FALSE;
		}
		return TRUE;
	}
	void DX9CaptureDATA::Destory()
	{
		if (m_bCopying)
		{
			LOG(INFO) << "[DX9CaptureDATA] Destory UnlockRect\n";
			TinyAutoLock autolock(m_lock);
			m_renderTarget->UnlockRect();
			m_bCopying = FALSE;
		}
		m_query.Release();
		m_copySurface.Release();
		m_renderTarget.Release();
		SAFE_DELETE_ARRAY(m_bits);
		m_pitch = 0;
	}
	BOOL DX9CaptureDATA::Update()
	{
		return TRUE;
	}
	BOOL DX9CaptureDATA::Enter()
	{
		return m_lock.Lock();
	}
	void DX9CaptureDATA::Leave()
	{
		m_lock.Unlock();
	}
	void DX9CaptureDATA::SetCopying(BOOL bCopying)
	{
		m_bCopying = bCopying;
	}
	BOOL DX9CaptureDATA::IsCopying() const
	{
		return m_bCopying;
	}
	void DX9CaptureDATA::SetIssue(BOOL bIssue)
	{
		m_bIssue = bIssue;
	}
	BOOL DX9CaptureDATA::IsIssue() const
	{
		return m_bIssue;
	}
	BYTE* DX9CaptureDATA::GetPointer() const
	{
		return m_bits;
	}
	UINT32 DX9CaptureDATA::GetPitch() const
	{
		return m_pitch;
	}
	IDirect3DSurface9*	DX9CaptureDATA::GetRenderTarget()
	{
		return m_renderTarget;
	}
	IDirect3DSurface9*	DX9CaptureDATA::GetCopySurface()
	{
		return m_copySurface;
	}
	IDirect3DQuery9*	DX9CaptureDATA::GetQuery()
	{
		return m_query;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9GraphicsCapture::DX9GraphicsCapture(DX& dx)
		:m_d3dFormat(D3DFMT_UNKNOWN),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_currentPointer(NULL),
		m_currentCapture(0),
		m_currentCPUTexture(0),
		m_dwCopy(0),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_copyBits(NULL),
		m_patchType(0),
		m_hD3D9(NULL),
		m_dx(dx)
	{
		m_textures[0] = m_textures[1] = NULL;
		m_copy.CreateEvent();
		m_close.CreateEvent();
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i] = new DX9CaptureDATA();
		}
	}
	DX9GraphicsCapture::~DX9GraphicsCapture()
	{
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i]->Destory();
			SAFE_DELETE(m_captures[i]);
			m_captures[i] = NULL;
		}
	}
	BOOL DX9GraphicsCapture::Initialize(HWND hWND)
	{
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
			LOG(INFO) << "DX9Capture::d3d9 FAIL\n";
			return FALSE;
		}
		m_d3d10_1.Reset(TEXT("d3d10_1.dll"));
		if (!m_d3d10_1.IsValid())
		{
			LOG(INFO) << "DX9Capture::d3d10_1 FAIL\n";
			return FALSE;
		}
		m_dxgi.Reset(TEXT("dxgi.dll"));
		if (!m_dxgi.IsValid())
		{
			LOG(INFO) << "DX9Capture::dxgi FAIL\n";
			return FALSE;
		}
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hD3D9, TEXT("Direct3DCreate9Ex"));
		if (!d3d9CreateEx)
		{
			LOG(INFO) << "DX9Capture::d3d9CreateEx FAIL\n";
			return FALSE;
		}
		TinyComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
		{
			LOG(INFO) << "DX9Capture::Initialize d3d9CreateEx() FAIL\n";
			return FALSE;
		}
		LOG(INFO) << "DX9Capture::Initialize d3d9CreateEx() OK\n";
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
			LOG(INFO) << "d3d9ex->CreateDeviceEx FAIL\n";
			return FALSE;
		}
		LOG(INFO) << "d3d9ex->CreateDeviceEx OK\n";
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX9EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)DX9EndScene))
		{
			if (m_dX9EndScene.BeginDetour())
			{
				LOG(INFO) << "m_dX9EndScene.BeginDetour()\n";
				return TRUE;
			}
		}
		LOG(INFO) << "m_dX9EndScene.BeginDetour() FAIL\n";
		return FALSE;
	}
	void DX9GraphicsCapture::QueryCopy(IDirect3DDevice9 *device)
	{
		HRESULT hRes = S_OK;
		for (UINT i = 0; i < NUM_BUFFERS; i++)
		{
			DX9CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->IsIssue())
				continue;
			hRes = pDATA->GetQuery()->GetData(0, 0, 0);
			if (FAILED(hRes))
				continue;
			pDATA->SetIssue(FALSE);
			IDirect3DSurface9 *copy = pDATA->GetCopySurface();
			D3DLOCKED_RECT lockedRect;
			hRes = copy->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
			if (SUCCEEDED(hRes))
			{
				pDATA->SetCopying(TRUE);
				m_copyBits = lockedRect.pBits;
				m_currentCPUTexture = i;
				m_copy.SetEvent();
			}
			break;
		}
	}
	BOOL DX9GraphicsCapture::Render(IDirect3DDevice9 *d3d)
	{
		ASSERT(d3d);
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "DX9Capture m_stop\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			LOG(INFO) << "DX9Capture m_start" << " m_bTextures:" << m_bTextures;
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
						m_dxgiFormat = GetDXGIFormat9(sd.Format);
						m_captureDATA.Format = sd.Format;
						m_captureDATA.Size.cx = sd.Width;
						m_captureDATA.Size.cy = sd.Height;
						if (m_bD3D9EX)
						{
							m_bTextures = DX9GPUHook(d3d);
						}
						else
						{
							m_bTextures = DX9CPUHook(d3d);
						}
					}
				}
			}
			if (m_bTextures)
			{
				if (m_bD3D9EX)
				{
					TinyComPtr<IDirect3DSurface9> backBuffer;
					if (FAILED(d3d->GetRenderTarget(0, &backBuffer)))
					{
						if (FAILED(d3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
						{
							return FALSE;
						}
					}
					if (!backBuffer || FAILED(d3d->StretchRect(backBuffer, NULL, m_surface, NULL, D3DTEXF_NONE)))
					{
						return FALSE;
					}
				}
				else
				{
					QueryCopy(d3d);
					INT32 nextCapture = (m_currentCapture == NUM_BUFFERS - 1) ? 0 : (m_currentCapture + 1);
					DX9CaptureDATA* pDATA1 = m_captures[m_currentCapture];
					IDirect3DSurface9 *copy = pDATA1->GetRenderTarget();
					TinyComPtr<IDirect3DSurface9> backBuffer;
					if (FAILED(d3d->GetRenderTarget(0, &backBuffer)))
					{
						if (FAILED(d3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
						{
							return FALSE;
						}
					}
					if (!backBuffer || FAILED(d3d->StretchRect(backBuffer, NULL, copy, NULL, D3DTEXF_NONE)))
					{
						return FALSE;
					}
					if (m_dwCopy < NUM_BUFFERS - 1)
					{
						m_dwCopy++;
					}
					else
					{
						DX9CaptureDATA* pDATA2 = m_captures[nextCapture];
						IDirect3DSurface9 *src = pDATA2->GetRenderTarget();
						IDirect3DSurface9 *dst = pDATA2->GetCopySurface();
						pDATA2->Enter();
						if (pDATA2->IsCopying())
						{
							dst->UnlockRect();
							pDATA2->SetCopying(FALSE);
						}
						pDATA2->Leave();
						d3d->GetRenderTargetData(src, dst);
						pDATA2->GetQuery()->Issue(D3DISSUE_END);
						pDATA2->SetIssue(TRUE);
					}
					m_currentCapture = nextCapture;
				}
			}
		}
		return TRUE;
	}
	void DX9GraphicsCapture::Reset()
	{
		LOG(INFO) << "[DX9GraphicsCapture] Reset\n";
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_surface.Release();
		m_resource.Release();
		m_d3d10.Release();
		m_copy.SetEvent();
		m_close.SetEvent();
		m_captureTask.Close(500);
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX9CaptureDATA* pDATA = m_captures[i];
			pDATA->Destory();
		}
		m_dx.m_textureMemery.Unmap();
		m_dx.m_textureMemery.Close();
	}
	BOOL DX9GraphicsCapture::Setup(IDirect3DDevice9 *pThis)
	{
		TinyComPtr<IDirect3D9> d3d;
		if (SUCCEEDED(pThis->GetDirect3D(&d3d)))
		{
			TinyComPtr<IDirect3D9> d3d9ex;
			if (SUCCEEDED(d3d->QueryInterface(__uuidof(IDirect3D9Ex), (void**)&d3d9ex)))
			{
				m_bD3D9EX = TRUE;
			}
		}
		TinyComPtr<IDirect3DSwapChain9> swapChain;
		if (SUCCEEDED(pThis->GetSwapChain(0, &swapChain)))
		{
			D3DPRESENT_PARAMETERS pp;
			if (SUCCEEDED(swapChain->GetPresentParameters(&pp)))
			{
				m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEXTURE;
				m_captureDATA.Format = pp.BackBufferFormat;
				m_captureDATA.Size.cx = pp.BackBufferWidth;
				m_captureDATA.Size.cy = pp.BackBufferHeight;
				m_captureDATA.HwndCapture = pp.hDeviceWindow;
				m_dx.SetWindowsHook();
				m_dX9Present.Initialize(GetVTable(pThis, (68 / 4)), (FARPROC)DX9Present);
				m_dX9Reset.Initialize(GetVTable(pThis, (64 / 4)), (FARPROC)DX9Reset);
				m_dX9SwapPresent.Initialize(GetVTable(swapChain, (12 / 4)), (FARPROC)DX9SwapPresent);
				if (m_bD3D9EX)
				{
					m_dX9PresentEx.Initialize(GetVTable(pThis, (484 / 4)), (FARPROC)DX9PresentEx);
					m_dX9ResetEx.Initialize(GetVTable(pThis, (528 / 4)), (FARPROC)DX9ResetEx);
				}
				m_dX9Present.BeginDetour();
				m_dX9Reset.BeginDetour();
				m_dX9SwapPresent.BeginDetour();
				if (m_bD3D9EX)
				{
					m_dX9PresentEx.BeginDetour();
					m_dX9ResetEx.BeginDetour();
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL DX9GraphicsCapture::DX9CPUHook(IDirect3DDevice9 *device)
	{
		m_captureDATA.CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		m_captureDATA.bFlip = FALSE;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX9CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->Create(device, m_captureDATA.Size.cx, m_captureDATA.Size.cy, (D3DFORMAT)m_captureDATA.Format))
				return FALSE;
		}
		m_captureDATA.Pitch = m_captures[0]->GetPitch();
		UINT size1 = (sizeof(SharedTextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (m_captureDATA.Pitch * m_captureDATA.Size.cy + 15) & 0xFFFFFFF0;
		m_captureDATA.MapSize = size1 + size2 * 2;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
		sharedTexture->Texture1Offset = size1;
		sharedTexture->Texture2Offset = size1 + size2;
		sharedTexture->FrameTime = 0;
		sharedTexture->TextureHandle = NULL;
		BYTE* ps = m_dx.GetSharedTexture(m_captureDATA.MapSize);
		m_textures[0] = ps + sharedTexture->Texture1Offset;
		m_textures[1] = ps + sharedTexture->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX9GraphicsCapture::OnMessagePump, this));
		m_dx.m_ready.SetEvent();
		return TRUE;
	}
	void DX9GraphicsCapture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { m_copy,m_close };
		DWORD dwCurrentID = 0;
		for (;;)
		{
			UINT32 currentCPUTexture = 0;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}
			DWORD dwNextID = dwCurrentID == 0 ? 1 : 0;
			currentCPUTexture = m_currentCPUTexture;
			LPVOID bits = m_copyBits;
			if (currentCPUTexture < NUM_BUFFERS && !!bits)
			{
				DX9CaptureDATA* pDATA = m_captures[currentCPUTexture];
				pDATA->Enter();
				do
				{
					SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
					if (m_dx.m_mutes[dwCurrentID].Lock(0))
					{
						memcpy(m_textures[dwCurrentID], bits, sharedCapture->Pitch * sharedCapture->Size.cy);
						SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
						sharedTexture->CurrentID = dwCurrentID;
						m_dx.m_mutes[dwCurrentID].Unlock();
						break;
					}
					if (m_dx.m_mutes[dwNextID].Lock(0))
					{
						memcpy(m_textures[dwNextID], bits, sharedCapture->Pitch * sharedCapture->Size.cy);
						SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
						sharedTexture->CurrentID = dwNextID;
						m_dx.m_mutes[dwNextID].Unlock();
						break;
					}
				} while (0);
				pDATA->Leave();
				dwCurrentID = dwNextID;
			}
		}
	}
	BOOL DX9GraphicsCapture::DX9GPUHook(IDirect3DDevice9 *pThis)
	{
		LOG(INFO) << "[DX9Capture] DX9GPUHook\n";
		ASSERT(m_hD3D9);
		PFN_D3D10_CREATE_DEVICE1 d3d10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1)m_d3d10_1.GetFunctionPointer(TEXT("D3D10CreateDevice1"));
		if (!d3d10CreateDevice1)
		{
			LOG(ERROR) << "[DX9GPUHook] D3D10CreateDevice1 FAIL\n";
			return FALSE;
		}
		CREATEDXGIFACTORY1PROC createDXGIFactory1 = (CREATEDXGIFACTORY1PROC)m_dxgi.GetFunctionPointer(TEXT("CreateDXGIFactory1"));
		if (!createDXGIFactory1)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateDXGIFactory1 FAIL\n";
			return FALSE;
		}
		TinyComPtr<IDXGIFactory1> factory;
		if (FAILED((*createDXGIFactory1)(__uuidof(IDXGIFactory1), (void**)&factory)))
		{
			LOG(ERROR) << "[DX9GPUHook] IDXGIFactory1 FAIL\n";
			return FALSE;
		}
		TinyComPtr<IDXGIAdapter1> adapter;
		if (FAILED(factory->EnumAdapters1(0, &adapter)))
		{
			LOG(ERROR) << "[DX9GPUHook] EnumAdapters1 FAIL\n";
			return FALSE;
		}
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_d3d10)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d10)))
			{
				LOG(ERROR) << "[DX9GPUHook] d3d10CreateDevice1 FAIL\n";
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
		HRESULT hRes = m_d3d10->CreateTexture2D(&desc, NULL, &d3d10Texture2D);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateTexture2D FAIL\n";
			return FALSE;
		}
		hRes = d3d10Texture2D->QueryInterface(__uuidof(ID3D10Resource), (void**)&m_resource);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] ID3D10Resource FAIL\n";
			return FALSE;
		}
		TinyComPtr<IDXGIResource> dxgiResource;
		hRes = d3d10Texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] IDXGIResource FAIL\n";
			return FALSE;
		}
		hRes = dxgiResource->GetSharedHandle(&m_hTextureHandle);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] GetSharedHandle FAIL\n";
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
				LOG(ERROR) << "[DX9GPUHook] VirtualProtect FAIL\n";
				return FALSE;
			}
			memcpy(patchData.Ptr(), patchAddress, patchSize);
			memcpy(patchAddress, patch[m_patchType - 1].patchData, patchSize);
		}
		LOG(INFO) << "[DX9GPUHook] m_captureDATA " << "cx:" << m_captureDATA.Size.cx << " cy:" << m_captureDATA.Size.cy;
		TinyComPtr<IDirect3DTexture9> d3d9Texture;
		hRes = pThis->CreateTexture(m_captureDATA.Size.cx, m_captureDATA.Size.cy, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_d3dFormat, D3DPOOL_DEFAULT, &d3d9Texture, &m_hTextureHandle);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateTexture FAIL\n";
			return FALSE;
		}
		if (patchAddress)
		{
			memcpy(patchAddress, patchData.Ptr(), patchSize);
			VirtualProtect(patchAddress, patchSize, dwOldProtect, &dwOldProtect);
		}
		hRes = d3d9Texture->GetSurfaceLevel(0, &m_surface);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] GetSurfaceLevel FAIL\n";
			return FALSE;
		}
		m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		m_captureDATA.bFlip = FALSE;
		m_captureDATA.MapSize = sizeof(SharedTextureDATA);
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA();
		sharedTexture->TextureHandle = m_hTextureHandle;
		m_dx.m_ready.SetEvent();
		LOG(INFO) << "[DX9GPUHook] ok\n";
		return TRUE;
	}
}