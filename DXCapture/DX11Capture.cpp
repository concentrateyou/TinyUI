#include "stdafx.h"
#include "DX11Capture.h"

namespace DXCapture
{
	DX11Capture::DX11Capture()
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE)
	{
	}

	DX11Capture::~DX11Capture()
	{

	}
	DX11Capture& DX11Capture::Instance()
	{
		static DX11Capture instance;
		return instance;
	}
	BOOL DX11Capture::Initialize(HWND hWND)
	{
		if (!DXGlobal::Instance().BuildEvents())
			return FALSE;
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d11.dll"));
		m_hD3D11 = GetModuleHandle(szD3DPath);
		if (m_hD3D11 == NULL)
			return FALSE;
		D3D11CREATEPROC d3d11Create = (D3D11CREATEPROC)GetProcAddress(m_hD3D11, TEXT("D3D11CreateDeviceAndSwapChain"));
		if (!d3d11Create)
			return FALSE;
		D3D_FEATURE_LEVEL levels[6] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 2;
		swapDesc.BufferDesc.Width = 1;
		swapDesc.BufferDesc.Height = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = hWND;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.Windowed = TRUE;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0;
		TinyComPtr<ID3D11Device> device;
		TinyComPtr<ID3D11DeviceContext>	immediateContext;
		TinyComPtr<IDXGISwapChain> swap;
		D3D_FEATURE_LEVEL level;
		if (FAILED(hRes = (*d3d11Create)(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, levels, 6, D3D11_SDK_VERSION, &swapDesc, &swap, &device, &level, &immediateContext)))
			return FALSE;
		ULONG *vtable = *(ULONG**)swap.Ptr();
		if (!m_dxPresent.Initialize((FARPROC)*(vtable + (32 / 4)), (FARPROC)DXGISwapPresent))
			return FALSE;
		if (!m_dxPresent.BeginDetour())
			return FALSE;
		if (!m_dxResizeBuffers.Initialize((FARPROC)*(vtable + (52 / 4)), (FARPROC)DXGISwapResizeBuffers))
			return FALSE;
		if (!m_dxResizeBuffers.BeginDetour())
			return FALSE;
		return TRUE;
	}
	void DX11Capture::Reset(BOOL bRelease)
	{
		m_bDetour = FALSE;
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		if (bRelease)
			m_resource.Release();
		else
			m_resource.Detach();
	}
	BOOL DX11Capture::Setup(IDXGISwapChain *swap)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device);
		if (FAILED(hRes))
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (FAILED(hRes))
			return FALSE;
		SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		sharedCapture->Format = (DWORD)scd.BufferDesc.Format;
		sharedCapture->Size.cx = scd.BufferDesc.Width;
		sharedCapture->Size.cy = scd.BufferDesc.Height;
		sharedCapture->HwndCapture = scd.OutputWindow;
		sharedCapture->bMultisample = scd.SampleDesc.Count > 1;
		m_dxgiFormat = scd.BufferDesc.Format;
		DXGlobal::Instance().Hook();
		return TRUE;
	}
	BOOL DX11Capture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
		ASSERT(sharedCapture);
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
		TinyComPtr<ID3D11Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bTextures)
			{
				m_bTextures = DX11GPUHook(device);
			}
			if (m_bTextures)
			{
				TinyComPtr<ID3D11DeviceContext> context;
				device->GetImmediateContext(&context);
				TinyComPtr<ID3D11Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
				{
					if (sharedCapture->bMultisample)
					{
						context->ResolveSubresource(m_resource, 0, backBuffer, 0, m_dxgiFormat);
					}
					else
					{
						context->CopyResource(m_resource, backBuffer);
					}
				}
			}
		}
		return TRUE;
	}

	BOOL DX11Capture::DX11GPUHook(ID3D11Device *device)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = DXGlobal::Instance().GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		D3D11_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = sharedCapture->Size.cx;
		texGameDesc.Height = sharedCapture->Size.cy;
		texGameDesc.MipLevels = 1;
		texGameDesc.ArraySize = 1;
		texGameDesc.Format = (DXGI_FORMAT)sharedCapture->Format;
		texGameDesc.SampleDesc.Count = 1;
		texGameDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texGameDesc.Usage = D3D11_USAGE_DEFAULT;
		texGameDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D11Texture2D> dx11Texture2D;
		if (FAILED(hRes = device->CreateTexture2D(&texGameDesc, NULL, &dx11Texture2D)))
			return FALSE;
		if (FAILED(hRes = dx11Texture2D->QueryInterface(__uuidof(ID3D11Resource), (void**)&m_resource)))
			return FALSE;
		TinyComPtr<IDXGIResource> resource;
		if (FAILED(hRes = dx11Texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->GetSharedHandle(&m_hTextureHandle)))
			return FALSE;
		if (SharedTextureDATA* sharedTexture = DXGlobal::Instance().GetSharedTextureDATA())
		{
			sharedTexture->TextureHandle = m_hTextureHandle;
		}
		sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
		sharedCapture->bFlip = FALSE;
		if (DXGlobal::Instance().m_ready)
			DXGlobal::Instance().m_ready.SetEvent();
		return TRUE;
	}
	HRESULT STDMETHODCALLTYPE DX11Capture::DXGISwapPresent(IDXGISwapChain *pThis, UINT syncInterval, UINT flags)
	{
		DX11Capture::Instance().m_dxPresent.EndDetour();
		DX11Capture::Instance().m_lock.Lock();
		if (!DX11Capture::Instance().m_bDetour)
		{
			DX11Capture::Instance().m_bDetour = TRUE;
			DX11Capture::Instance().Setup(pThis);
		}
		if ((flags & DXGI_PRESENT_TEST) == 0)
		{
			DX11Capture::Instance().Render(pThis, flags);
		}
		DX11Capture::Instance().m_lock.Unlock();
		HRESULT hRes = pThis->Present(syncInterval, flags);
		DX11Capture::Instance().m_dxPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX11Capture::DXGISwapResizeBuffers(IDXGISwapChain *pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		DX11Capture::Instance().m_dxResizeBuffers.EndDetour();
		DX11Capture::Instance().m_lock.Lock();
		DX11Capture::Instance().Reset();
		DX11Capture::Instance().m_lock.Unlock();
		HRESULT hRes = pThis->ResizeBuffers(bufferCount, width, height, giFormat, flags);
		DX11Capture::Instance().m_dxResizeBuffers.BeginDetour();
		return hRes;
	}
}
