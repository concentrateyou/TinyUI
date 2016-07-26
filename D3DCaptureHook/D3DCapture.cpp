#include "stdafx.h"
#include "D3DCapture.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

typedef IDirect3D9* (WINAPI*D3D9CREATEPROC)(UINT);
typedef HRESULT(WINAPI*D3D9CREATEEXPROC)(UINT, IDirect3D9Ex**);
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
INT GetD3D9PatchType(HMODULE hModule)
{
	LPBYTE lpBaseAddress = (LPBYTE)hModule;
	for (int i = 0; i < NUM_KNOWN_PATCHES; i++)
	{
		if (CompareMemory(lpBaseAddress + patch_offsets[i], patch_compare[i], PATCH_COMPARE_SIZE))
			return i + 1;
	}

	return 0;
}
LPBYTE GetD3D9PatchAddress(HMODULE hModule, INT patchType)
{
	if (patchType)
	{
		LPBYTE lpBaseAddress = (LPBYTE)hModule;
		return lpBaseAddress + patch_offsets[patchType - 1] + PATCH_COMPARE_SIZE;
	}
	return NULL;
}

namespace D3D
{
	D3D9Capture::D3D9Capture()
		:m_hD3D9DLL(NULL),
		m_d3dFormat(D3DFMT_UNKNOWN),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_pSharedHandle(NULL),
		m_pGameDevice(NULL),
		m_pSharedTextureData(NULL),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_bUseSharedTextures(FALSE),
		m_lastTime(0),
		m_patchType(0),
		m_dwCurrentCapture(0)
	{

	}
	D3D9Capture::~D3D9Capture()
	{
		m_d3d10Resource->Release();
		m_d3d9TextureGame->Release();
		m_d3d10Device1->Release();
	}
	D3D9Capture& D3D9Capture::Instance()
	{
		static D3D9Capture instance;
		return instance;
	}
	BOOL D3D9Capture::Initialize(HWND hWND)
	{
		DWORD dwProcessID = GetCurrentProcessId();
		stringstream stream;
		stream << BEGIN_CAPTURE_EVENT << dwProcessID;
		if (!m_beginCapture.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_beginCapture.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		stream.clear();
		stream << END_CAPTURE_EVENT << dwProcessID;
		if (!m_endCapture.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_endCapture.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d9.dll"));
		m_hD3D9DLL = GetModuleHandle(szD3DPath);
		if (!m_hD3D9DLL)
		{
			return FALSE;
		}
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hD3D9DLL, "Direct3DCreate9Ex");
		if (!d3d9CreateEx)
		{
			return FALSE;
		}
		CComPtr<IDirect3D9Ex> d3d9ex;
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
	BOOL D3D9Capture::D3D9Draw(IDirect3DDevice9 *device)
	{
		ASSERT(device);
		if (m_bCapturing && m_endCapture.Lock(0))
		{
			m_bCapturing = FALSE;
			return FALSE;
		}
		if (!m_bCapturing && m_beginCapture.Lock(0))
		{
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bTextures)
			{
				m_patchType = GetD3D9PatchType(m_hD3D9DLL);
				CComPtr<IDirect3DSurface9> backBuffer;
				if (SUCCEEDED(device->GetRenderTarget(0, &backBuffer)))
				{
					D3DSURFACE_DESC sd;
					::ZeroMemory(&sd, sizeof(sd));
					if (SUCCEEDED(backBuffer->GetDesc(&sd)))
					{
						m_d3dFormat = sd.Format;
						m_dxgiFormat = GetDXGIFormat(sd.Format);
						m_d3dSharedTexture.Format = sd.Format;
						m_d3dSharedTexture.Width = sd.Width;
						m_d3dSharedTexture.Height = sd.Height;
						m_bTextures = D3D9GPUHook(device);
					}
				}
			}
			if (m_bTextures)
			{
				DWORD dwNextCapture = m_dwCurrentCapture == 0 ? 1 : 0;
				CComPtr<IDirect3DSurface9> backBuffer;
				if (FAILED(device->GetRenderTarget(0, &backBuffer)))
				{
					if (FAILED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
					{
						return FALSE;
					}
				}
				if (!backBuffer || FAILED(device->StretchRect(backBuffer, NULL, m_d3d9TextureGame, NULL, D3DTEXF_NONE)))
				{
					return FALSE;
				}
				m_dwCurrentCapture = dwNextCapture;
			}
		}
		return TRUE;
	}
	void D3D9Capture::D3DReset()
	{
		m_bTextures = FALSE;
		m_d3d10Resource->Release();
		m_d3d9TextureGame->Release();
		m_d3d10Device1->Release();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
		m_pSharedTextureData = NULL;
	}
	BOOL D3D9Capture::InitializeSharedMemoryGPUCapture(SharedTextureData** textureData)
	{
		INT size = sizeof(SharedTextureData);
		m_textureMemery.Unmap();
		m_textureMemery.Close();
		if (!m_textureMemery.Create(TEXTURE_MEMORY, size))
			return FALSE;
		if (!m_textureMemery.Map(0, 0))
			return FALSE;
		*textureData = (SharedTextureData*)m_textureMemery.Address();
		(*textureData)->FrameTime = 0;
		return TRUE;
	}
	BOOL D3D9Capture::D3D9GPUHook(IDirect3DDevice9 *device)
	{
		CScopedLibrary d3d10_1(TEXT("d3d10_1.dll"));
		if (!d3d10_1.IsValid())
		{
			return FALSE;
		}
		CScopedLibrary dxgi(TEXT("dxgi.dll"));
		if (!dxgi.IsValid())
		{
			return FALSE;
		}
		PFN_D3D10_CREATE_DEVICE1 d3d10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1)GetProcAddress(d3d10_1, "D3D10CreateDevice1");
		if (!d3d10CreateDevice1)
		{
			return FALSE;
		}
		CREATEDXGIFACTORY1PROC createDXGIFactory1 = (CREATEDXGIFACTORY1PROC)GetProcAddress(dxgi, "CreateDXGIFactory1");
		if (!createDXGIFactory1)
		{
			return FALSE;
		}
		CComPtr<IDXGIFactory1> factory;
		if (FAILED((*createDXGIFactory1)(__uuidof(IDXGIFactory1), (void**)&factory)))
		{
			return FALSE;
		}
		CComPtr<IDXGIAdapter1> adapter;
		if (FAILED(factory->EnumAdapters1(0, &adapter)))
		{
			return FALSE;
		}
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_d3d10Device1)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d10Device1)))
			{
				return FALSE;
			}
		}
		D3D10_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = m_d3dSharedTexture.Width;
		texGameDesc.Height = m_d3dSharedTexture.Height;
		texGameDesc.MipLevels = 1;
		texGameDesc.ArraySize = 1;
		texGameDesc.Format = m_dxgiFormat;
		texGameDesc.SampleDesc.Count = 1;
		texGameDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		texGameDesc.Usage = D3D10_USAGE_DEFAULT;
		texGameDesc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
		CComPtr<ID3D10Texture2D> d3d101Tex;
		if (FAILED(m_d3d10Device1->CreateTexture2D(&texGameDesc, NULL, &d3d101Tex)))
		{
			return FALSE;
		}
		if (FAILED(d3d101Tex->QueryInterface(__uuidof(ID3D10Resource), (void**)&m_d3d10Resource)))
		{
			return FALSE;
		}
		CComPtr<IDXGIResource> dxgiResource;
		if (FAILED(d3d101Tex->QueryInterface(IID_IDXGIResource, (void**)&dxgiResource)))
		{
			return FALSE;
		}
		if (FAILED(dxgiResource->GetSharedHandle(&m_pSharedHandle)))
		{
			return FALSE;
		}
		LPBYTE patchAddress = (m_patchType != 0) ? GetD3D9PatchAddress(m_hD3D9DLL, m_patchType) : NULL;
		DWORD dwOldProtect;
		size_t patchSize;
		CScopedArray<BYTE> patchData;
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
		CComPtr<IDirect3DTexture9> d3d9Texture;
		if (FAILED(device->CreateTexture(m_d3dSharedTexture.Width, m_d3dSharedTexture.Height, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_d3dFormat, D3DPOOL_DEFAULT, &d3d9Texture, &m_pSharedHandle)))
		{
			return FALSE;
		}
		if (patchAddress)
		{
			memcpy(patchAddress, patchData.Ptr(), patchSize);
			VirtualProtect(patchAddress, patchSize, dwOldProtect, &dwOldProtect);
		}
		if (FAILED(d3d9Texture->GetSurfaceLevel(0, &m_d3d9TextureGame)))
		{
			return FALSE;
		}
		if (!InitializeSharedMemoryGPUCapture(&m_pSharedTextureData))
		{
			return FALSE;
		}
		m_d3dSharedTexture.CaptureType = CAPTURETYPE_SHAREDTEX;
		m_d3dSharedTexture.bFlip = FALSE;
		m_pSharedTextureData->TextureHandle = m_pSharedHandle;
		return TRUE;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9EndScene(IDirect3DDevice9 *device)
	{
		D3D9Capture::Instance().m_lock.Acquire();
		D3D9Capture::Instance().m_d3d9EndScene.EndDetour();
		HRESULT hRes = S_OK;
		if (!D3D9Capture::Instance().m_pGameDevice)
		{
			D3D9Capture::Instance().m_pGameDevice = device;
			CComPtr<IDirect3DSwapChain9> swapChain;
			if (SUCCEEDED(device->GetSwapChain(0, &swapChain)))
			{
				D3DPRESENT_PARAMETERS pp;
				if (SUCCEEDED(swapChain->GetPresentParameters(&pp)))
				{
					D3D9Capture::Instance().m_d3dSharedTexture.CaptureType = CAPTURETYPE_SHAREDTEX;
					D3D9Capture::Instance().m_d3dSharedTexture.Format = pp.BackBufferFormat;
					D3D9Capture::Instance().m_d3dSharedTexture.Width = pp.BackBufferWidth;
					D3D9Capture::Instance().m_d3dSharedTexture.Height = pp.BackBufferHeight;
					D3D9Capture::Instance().m_d3dSharedTexture.HwndCapture = pp.hDeviceWindow;
					D3D9Capture::Instance().m_d3d9PresentEx.Initialize(GetVTable(device, (484 / 4)), (FARPROC)D3D9PresentEx);
					D3D9Capture::Instance().m_d3d9PresentEx.BeginDetour();
					D3D9Capture::Instance().m_d3d9ResetEx.Initialize(GetVTable(device, (528 / 4)), (FARPROC)D3D9ResetEx);
					D3D9Capture::Instance().m_d3d9ResetEx.BeginDetour();
					D3D9Capture::Instance().m_d3d9Present.Initialize(GetVTable(device, (68 / 4)), (FARPROC)D3D9Present);
					D3D9Capture::Instance().m_d3d9Present.BeginDetour();
					D3D9Capture::Instance().m_d3d9Reset.Initialize(GetVTable(device, (64 / 4)), (FARPROC)D3D9Reset);
					D3D9Capture::Instance().m_d3d9Reset.BeginDetour();
					D3D9Capture::Instance().m_d3d9SwapPresent.Initialize(GetVTable(swapChain, (12 / 4)), (FARPROC)D3D9SwapPresent);
					D3D9Capture::Instance().m_d3d9SwapPresent.BeginDetour();
					D3D9Capture::Instance().m_lastTime = 0;
					D3D9Capture::Instance().m_timer.BeginTime();
				}
			}
		}
		hRes = device->EndScene();
		D3D9Capture::Instance().m_d3d9EndScene.BeginDetour();
		D3D9Capture::Instance().m_lock.Release();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9Present(IDirect3DDevice9 *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		D3D9Capture::Instance().m_d3d9Present.EndDetour();
		D3D9Capture::Instance().D3D9Draw(device);
		HRESULT hRes = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		D3D9Capture::Instance().m_d3d9Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9PresentEx(IDirect3DDevice9Ex *device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
	{
		D3D9Capture::Instance().m_d3d9PresentEx.EndDetour();
		D3D9Capture::Instance().D3D9Draw(device);
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
		D3D9Capture::Instance().D3DReset();
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