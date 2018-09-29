#include "stdafx.h"
#include "DX11GraphicsCapture.h"
#include <wincodec.h>

namespace GraphicsCapture
{
	DX11GraphicsCapture::DX11GraphicsCapture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_handle(NULL),
		m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_dx(dx)
	{
	}

	DX11GraphicsCapture::~DX11GraphicsCapture()
	{

	}
	BOOL DX11GraphicsCapture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
	{
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
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 2;
		desc.BufferDesc.Width = 1;
		desc.BufferDesc.Height = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = hWND;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = TRUE;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;
		TinyComPtr<ID3D11Device> device;
		TinyComPtr<ID3D11DeviceContext>	immediateContext;
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_9_3;
		if (FAILED(hRes = (*d3d11Create)(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, levels, 6, D3D11_SDK_VERSION, &desc, &swap, &device, &level, &immediateContext)))
			return FALSE;
		return TRUE;
	}
	void DX11GraphicsCapture::Reset()
	{
		m_bActive = FALSE;
		m_handle = NULL;
		m_texture2D.Release();
		m_dx.m_textureDATA.Close();
	}
	BOOL DX11GraphicsCapture::Setup(IDXGISwapChain *swap)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device);
		if (hRes != S_OK)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (hRes != S_OK)
			return FALSE;
		m_dxgiFormat = GetDXTextureFormat(scd.BufferDesc.Format);
		hookDATA->Format = m_dxgiFormat;
		hookDATA->Size.cx = scd.BufferDesc.Width;
		hookDATA->Size.cy = scd.BufferDesc.Height;
		hookDATA->Window = scd.OutputWindow;
		hookDATA->bMultisample = scd.SampleDesc.Count > 1;
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX11GraphicsCapture::Draw(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D11Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bActive)
			{
				m_bActive = DX11GPUHook(device);
			}
			if (m_bActive)
			{
				TinyComPtr<ID3D11DeviceContext> context;
				device->GetImmediateContext(&context);
				TinyComPtr<ID3D11Texture2D> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)))
				{
					HookDATA* hookDATA = m_dx.GetHookDATA();
					ASSERT(hookDATA);
					if (hookDATA->bMultisample)
					{
						context->ResolveSubresource(m_texture2D, 0, backBuffer, 0, m_dxgiFormat);
					}
					else
					{
						context->CopyResource(m_texture2D, backBuffer);
					}
				}
			}
		}
		return TRUE;
	}
	BOOL DX11GraphicsCapture::DX11CPUHook(ID3D11Device *swap)
	{
		return TRUE;
	}
	BOOL DX11GraphicsCapture::DX11GPUHook(ID3D11Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		HRESULT hRes = S_OK;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = hookDATA->Size.cx;
		desc.Height = hookDATA->Size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		if (FAILED(hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D)))
			return FALSE;
		TinyComPtr<IDXGIResource> dxgiResource;
		if (FAILED(hRes = m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
			return FALSE;
		if (FAILED(hRes = dxgiResource->GetSharedHandle(&m_handle)))
			return FALSE;
		hookDATA->CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapSize = sizeof(TextureDATA);
		if (!g_dx.CreateTextureDATA(hookDATA->MapID, hookDATA->MapSize))
			return FALSE;
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->TextureHandle = m_handle;
		m_dx.m_targetReady.SetEvent();
		return TRUE;
	}
}
