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
			g_dx9.Draw(device);
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
			g_dx9.Draw(device);
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
				g_dx9.Draw(device);
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
		m_bIssue(FALSE),
		m_pitch(0)
	{

	}
	DX9CaptureDATA::~DX9CaptureDATA()
	{
		Destory();
	}
	BOOL DX9CaptureDATA::Create(IDirect3DDevice9* device, INT cx, INT cy, D3DFORMAT format)
	{
		Destory();
		HRESULT hRes = S_OK;
		hRes = device->CreateOffscreenPlainSurface(cx, cy, (D3DFORMAT)format, D3DPOOL_SYSTEMMEM, &m_copy2D, NULL);
		if (FAILED(hRes))
		{
			LOG(ERROR) << "[DX9CaptureDATA] CreateOffscreenPlainSurface FAIL";
			return FALSE;
		}
		D3DLOCKED_RECT lockedRect;
		hRes = m_copy2D->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
		if (FAILED(hRes))
		{
			LOG(ERROR) << "[DX9CaptureDATA] LockRect FAIL";
			return FALSE;
		}
		m_pitch = lockedRect.Pitch;
		m_copy2D->UnlockRect();
		hRes = device->CreateRenderTarget(cx, cy, (D3DFORMAT)format, D3DMULTISAMPLE_NONE, 0, FALSE, &m_texture2D, NULL);
		if (FAILED(hRes))
		{
			LOG(ERROR) << "[DX9CaptureDATA] CreateRenderTarget FAIL";
			return FALSE;
		}
		hRes = device->CreateQuery(D3DQUERYTYPE_EVENT, &m_query);
		if (FAILED(hRes))
		{
			LOG(ERROR) << "[DX9CaptureDATA] CreateQuery FAIL";
			return FALSE;
		}
		return TRUE;
	}
	void DX9CaptureDATA::Destory()
	{
		if (m_bCopying)
		{
			LOG(INFO) << "[DX9CaptureDATA] Destory UnlockRect";
			m_texture2D->UnlockRect();
			m_bCopying = FALSE;
		}
		m_bIssue = FALSE;
		m_size.SetSize(0, 0);
		m_query.Release();
		m_copy2D.Release();
		m_texture2D.Release();
		m_pitch = 0;
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
	UINT32 DX9CaptureDATA::GetPitch() const
	{
		return m_pitch;
	}
	IDirect3DSurface9*	DX9CaptureDATA::GetTexture2D()
	{
		return m_texture2D;
	}
	IDirect3DSurface9*	DX9CaptureDATA::GetCopy2D()
	{
		return m_copy2D;
	}
	IDirect3DQuery9*	DX9CaptureDATA::GetQuery()
	{
		return m_query;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9GraphicsCapture::DX9GraphicsCapture(DX& dx)
		:m_d3dFormat(D3DFMT_UNKNOWN),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_handle(NULL),
		m_currentPointer(NULL),
		m_currentIndex(0),
		m_currentCopy(0),
		m_dwCopy(0),
		m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_bits(NULL),
		m_patchType(0),
		m_hD3D9(NULL),
		m_dx(dx)
	{
		m_textures[0] = m_textures[1] = NULL;
		m_copy.CreateEvent();
		m_stop.CreateEvent();
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
			LOG(ERROR) << "GetModuleHandle d3d9 FAIL";
			return FALSE;
		}
		m_d3d10_1.Reset(TEXT("d3d10_1.dll"));
		if (!m_d3d10_1.IsValid())
		{
			LOG(ERROR) << "LoadLibrary d3d10_1 FAIL";
			return FALSE;
		}
		m_dxgi.Reset(TEXT("dxgi.dll"));
		if (!m_dxgi.IsValid())
		{
			LOG(ERROR) << "LoadLibrary dxgi FAIL";
			return FALSE;
		}
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hD3D9, TEXT("Direct3DCreate9Ex"));
		if (!d3d9CreateEx)
		{
			LOG(ERROR) << "Query Direct3DCreate9Ex FAIL";
			return FALSE;
		}
		TinyComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
		{
			LOG(ERROR) << "Call Direct3DCreate9Ex FAIL";
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
			LOG(ERROR) << "Call CreateDeviceEx FAIL";
			return FALSE;
		}
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX9EndScene.Initialize((FARPROC)*(vtable + 42), (FARPROC)DX9EndScene))
		{
			if (m_dX9EndScene.BeginDetour())
			{
				return TRUE;
			}
		}
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
			IDirect3DSurface9 *copy = pDATA->GetCopy2D();
			D3DLOCKED_RECT s = { 0 };
			hRes = copy->LockRect(&s, NULL, D3DLOCK_READONLY);
			if (SUCCEEDED(hRes))
			{
				pDATA->SetCopying(TRUE);
				m_lock.Lock();
				m_bits = s.pBits;
				m_currentCopy = i;
				m_lock.Unlock();
				m_copy.SetEvent();
			}
			break;
		}
	}
	BOOL DX9GraphicsCapture::Draw(IDirect3DDevice9 *d3d)
	{
		ASSERT(d3d);
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "DX9Capture Stop";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			LOG(INFO) << "DX9Capture Start";
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bActive)
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
						HookDATA* hookDATA = m_dx.GetHookDATA();
						ASSERT(hookDATA);
						hookDATA->Format = sd.Format;
						hookDATA->Size.cx = sd.Width;
						hookDATA->Size.cy = sd.Height;
						if (m_bD3D9EX && !hookDATA->bCPU)
						{
							m_bActive = DX9GPUHook(d3d);
						}
						else
						{
							m_bActive = DX9CPUHook(d3d);
						}
					}
				}
			}
			if (m_bActive)
			{
				HookDATA* hookDATA = m_dx.GetHookDATA();
				ASSERT(hookDATA);
				if (m_bD3D9EX && !hookDATA->bCPU)
				{
					TinyComPtr<IDirect3DSurface9> backBuffer;
					if (FAILED(d3d->GetRenderTarget(0, &backBuffer)))
					{
						if (FAILED(d3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
						{
							return FALSE;
						}
					}
					if (!backBuffer || FAILED(d3d->StretchRect(backBuffer, NULL, m_copy2D, NULL, D3DTEXF_NONE)))
					{
						return FALSE;
					}
				}
				else
				{
					QueryCopy(d3d);
					INT32 next = (m_currentIndex == NUM_BUFFERS - 1) ? 0 : (m_currentIndex + 1);
					DX9CaptureDATA* pDATA1 = m_captures[m_currentIndex];
					IDirect3DSurface9 *copy2D = pDATA1->GetTexture2D();
					TinyComPtr<IDirect3DSurface9> backBuffer;
					if (FAILED(d3d->GetRenderTarget(0, &backBuffer)))
					{
						if (FAILED(d3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
						{
							return FALSE;
						}
					}
					if (!backBuffer || FAILED(d3d->StretchRect(backBuffer, NULL, copy2D, NULL, D3DTEXF_NONE)))
					{
						return FALSE;
					}
					if (m_dwCopy < NUM_BUFFERS - 1)
					{
						m_dwCopy++;
					}
					else
					{
						DX9CaptureDATA* pDATA2 = m_captures[next];
						IDirect3DSurface9 *src = pDATA2->GetTexture2D();
						IDirect3DSurface9 *dst = pDATA2->GetCopy2D();
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
					m_currentIndex = next;
				}
			}
		}
		return TRUE;
	}
	void DX9GraphicsCapture::Reset()
	{
		m_stop.SetEvent();
		m_captureTask.Close(500);
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX9CaptureDATA* pDATA = m_captures[i];
			pDATA->Destory();
		}
		m_dx.m_textureDATA.Close();
		m_dwCopy = 0;
		m_bits = NULL;
		m_textures[0] = m_textures[1] = NULL;
		m_currentIndex = 0;
		m_currentCopy = 0;
		m_bCapturing = FALSE;
		m_bActive = FALSE;
		m_handle = NULL;
		g_dx.m_start.SetEvent();
		m_copy2D.Release();
		m_texture2D.Release();
		m_d3d10.Release();
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
				HookDATA* hookDATA = m_dx.GetHookDATA();
				ASSERT(hookDATA);
				hookDATA->CaptureType = CAPTURETYPE_SHAREDTEXTURE;
				hookDATA->Format = pp.BackBufferFormat;
				hookDATA->Size.cx = pp.BackBufferWidth;
				hookDATA->Size.cy = pp.BackBufferHeight;
				hookDATA->Window = pp.hDeviceWindow;
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
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		hookDATA->CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapID++;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX9CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->Create(device, hookDATA->Size.cx, hookDATA->Size.cy, (D3DFORMAT)hookDATA->Format))
				return FALSE;
		}
		hookDATA->Pitch = m_captures[0]->GetPitch();
		LOG(INFO) << "[DX9CPUHook] Pitch:" << hookDATA->Pitch << " CX: " << hookDATA->Size.cx << " CY:" << hookDATA->Size.cy;
		UINT size1 = (sizeof(TextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (hookDATA->Pitch * hookDATA->Size.cy + 15) & 0xFFFFFFF0;
		hookDATA->MapSize = size1 + size2 * 2;
		TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapID, hookDATA->MapSize);
		if (!textureDATA)
			return FALSE;
		textureDATA->Texture1Offset = size1;
		textureDATA->Texture2Offset = size1 + size2;
		textureDATA->TextureHandle = NULL;
		BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
		m_textures[0] = address + textureDATA->Texture1Offset;
		m_textures[1] = address + textureDATA->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX9GraphicsCapture::OnMessagePump, this));
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX9CPUHook] OK";
		return TRUE;
	}
	void DX9GraphicsCapture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { m_copy, m_stop };
		DWORD dwCurrentID = 0;
		for (;;)
		{
			INT32 currentCopy = 0;
			LPVOID bits = NULL;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}
			m_lock.Lock();
			currentCopy = m_currentCopy;
			bits = m_bits;
			m_lock.Unlock();
			if (currentCopy < NUM_BUFFERS && !!bits)
			{
				DWORD dwNextID = dwCurrentID == 0 ? 1 : 0;
				DX9CaptureDATA* pDATA = m_captures[currentCopy];
				pDATA->Enter();
				do
				{
					HookDATA* hookDATA = m_dx.GetHookDATA();
					ASSERT(hookDATA);
					if (m_dx.m_mutes[dwCurrentID].Lock(0))
					{
						memcpy(m_textures[dwCurrentID], bits, hookDATA->Pitch * hookDATA->Size.cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapSize);
						textureDATA->CurrentID = dwCurrentID;
						dwCurrentID = dwCurrentID == 0 ? 1 : 0;
						m_dx.m_mutes[dwCurrentID].Unlock();
						break;
					}
					if (m_dx.m_mutes[dwNextID].Lock(0))
					{
						memcpy(m_textures[dwNextID], bits, hookDATA->Pitch * hookDATA->Size.cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapSize);
						textureDATA->CurrentID = dwNextID;
						dwCurrentID = dwNextID == 0 ? 1 : 0;;
						m_dx.m_mutes[dwNextID].Unlock();
						break;
					}
				} while (0);
				pDATA->Leave();

			}
		}
	}
	BOOL DX9GraphicsCapture::DX9GPUHook(IDirect3DDevice9 *pThis)
	{
		ASSERT(m_hD3D9);
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		PFN_D3D10_CREATE_DEVICE1 d3d10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1)m_d3d10_1.GetFunctionPointer(TEXT("D3D10CreateDevice1"));
		if (!d3d10CreateDevice1)
		{
			LOG(ERROR) << "[DX9GPUHook] D3D10CreateDevice1 FAIL";
			return FALSE;
		}
		CREATEDXGIFACTORY1PROC createDXGIFactory1 = (CREATEDXGIFACTORY1PROC)m_dxgi.GetFunctionPointer(TEXT("CreateDXGIFactory1"));
		if (!createDXGIFactory1)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateDXGIFactory1 FAIL";
			return FALSE;
		}
		TinyComPtr<IDXGIFactory1> factory;
		if (FAILED((*createDXGIFactory1)(__uuidof(IDXGIFactory1), (void**)&factory)))
		{
			LOG(ERROR) << "[DX9GPUHook] IDXGIFactory1 FAIL";
			return FALSE;
		}
		TinyComPtr<IDXGIAdapter1> adapter;
		if (FAILED(factory->EnumAdapters1(0, &adapter)))
		{
			LOG(ERROR) << "[DX9GPUHook] EnumAdapters1 FAIL";
			return FALSE;
		}
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_d3d10)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d10)))
			{
				LOG(ERROR) << "[DX9GPUHook] d3d10CreateDevice1 FAIL";
				return FALSE;
			}
		}
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = hookDATA->Size.cx;
		desc.Height = hookDATA->Size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
		HRESULT hRes = m_d3d10->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateTexture2D FAIL";
			return FALSE;
		}
		TinyComPtr<IDXGIResource> dxgiResource;
		hRes = m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] IDXGIResource FAIL";
			return FALSE;
		}
		hRes = dxgiResource->GetSharedHandle(&m_handle);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] GetSharedHandle FAIL";
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
				LOG(ERROR) << "[DX9GPUHook] VirtualProtect FAIL";
				return FALSE;
			}
			memcpy(patchData.Ptr(), patchAddress, patchSize);
			memcpy(patchAddress, patch[m_patchType - 1].patchData, patchSize);
		}
		TinyComPtr<IDirect3DTexture9> texture2D;
		hRes = pThis->CreateTexture(hookDATA->Size.cx, hookDATA->Size.cy, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_d3dFormat, D3DPOOL_DEFAULT, &texture2D, &m_handle);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] CreateTexture FAIL";
			return FALSE;
		}
		if (patchAddress)
		{
			memcpy(patchAddress, patchData.Ptr(), patchSize);
			VirtualProtect(patchAddress, patchSize, dwOldProtect, &dwOldProtect);
		}
		hRes = texture2D->GetSurfaceLevel(0, &m_copy2D);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "[DX9GPUHook] GetSurfaceLevel FAIL";
			return FALSE;
		}
		hookDATA->CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapSize = sizeof(TextureDATA);
		TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapID, hookDATA->MapSize);
		if (!textureDATA)
			return FALSE;
		textureDATA->TextureHandle = m_handle;
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX9GPUHook] OK";
		return TRUE;
	}
	BOOL DX9GraphicsCapture::hookable()
	{
		return !m_dX9Present.IsEmpty() &&
			!m_dX9PresentEx.IsEmpty() &&
			!m_dX9SwapPresent.IsEmpty();
	}
}