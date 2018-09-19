#include "stdafx.h"
#include "DX10GraphicsCapture.h"

namespace GraphicsCapture
{
	DX10GraphicsCapture::DX10GraphicsCapture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_hD3D10(NULL),
		m_dx(dx)
	{
	}

	DX10GraphicsCapture::~DX10GraphicsCapture()
	{

	}
	BOOL DX10GraphicsCapture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d10.dll"));
		m_hD3D10 = GetModuleHandle(szD3DPath);
		if (m_hD3D10 == NULL)
			return FALSE;
		D3D10CREATEPROC d3d10Create = (D3D10CREATEPROC)GetProcAddress(m_hD3D10, TEXT("D3D10CreateDeviceAndSwapChain"));
		if (!d3d10Create)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 2;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width = 1;
		swapDesc.BufferDesc.Height = 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = hWND;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.Windowed = TRUE;
		TinyComPtr<ID3D10Device> device;
		if (FAILED(hRes = (*d3d10Create)(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapDesc, &swap, &device)))
			return FALSE;
		LOG(INFO) << "DX10Capture::Initialize OK\n";
		return TRUE;
	}
	void DX10GraphicsCapture::Reset()
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_texture2D.Release();
		m_copy2D.Release();
		m_dx.m_textureDATA.Close();
	}
	BOOL DX10GraphicsCapture::Setup(IDXGISwapChain *swap)
	{
		LOG(INFO) << "DX10Capture Setup OK\n";
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device);
		if (hRes != S_OK)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (hRes != S_OK)
			return FALSE;
		m_dxgiFormat = GetDX10PlusTextureFormat(scd.BufferDesc.Format);
		m_captureDATA.Format = (DWORD)m_dxgiFormat;
		m_captureDATA.Size.cx = scd.BufferDesc.Width;
		m_captureDATA.Size.cy = scd.BufferDesc.Height;
		m_captureDATA.Window = scd.OutputWindow;
		m_captureDATA.bMultisample = scd.SampleDesc.Count > 1;
		if (m_captureDATA.bMultisample)
		{
			TinyComPtr<ID3D10Texture2D> backBuffer;
			hRes = swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
			if (hRes != S_OK)
				return FALSE;
			D3D10_TEXTURE2D_DESC desc;
			backBuffer->GetDesc(&desc);
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = scd.BufferDesc.Format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
			desc.Usage = D3D10_USAGE_DEFAULT;
			desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
			hRes = device->CreateTexture2D(&desc, NULL, &m_copy2D);
			if (hRes != S_OK)
				return FALSE;
		}
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX10GraphicsCapture::Draw(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "DX10Capture::Render m_stop OK\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D10Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bTextures)
			{
				m_bTextures = DX10GPUHook(device);
				LOG(INFO) << "DX10Capture::Render DX10GPUHook\n";
				LOG(INFO) << "DX10Capture::Render  swap->GetDevice m_bCapturing:" << m_bCapturing << " m_bTextures:" << m_bTextures << "\n";
				LOG(INFO) << "------------------------------------------------------------------------------------------------\n";
			}
			if (m_bTextures)
			{
				TinyComPtr<ID3D10Texture2D> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer)))
				{
					if (m_captureDATA.bMultisample)
					{
						if (m_copy2D != NULL)
						{
							D3D10_TEXTURE2D_DESC desc;
							backBuffer->GetDesc(&desc);
							device->ResolveSubresource(m_copy2D, 0, backBuffer, 0, desc.Format);
							device->CopyResource(m_texture2D, m_copy2D);
						}
					}
					else
					{
						device->CopyResource(m_texture2D, backBuffer);
					}
				}
			}
		}
		return TRUE;
	}
	BOOL DX10GraphicsCapture::DX10GPUHook(ID3D10Device *device)
	{
		HRESULT hRes = S_OK;
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
		if (FAILED(hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D)))
			return FALSE;
		TinyComPtr<IDXGIResource> dxgiResource;
		if (FAILED(hRes = m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
			return FALSE;
		if (FAILED(hRes = dxgiResource->GetSharedHandle(&m_hTextureHandle)))
			return FALSE;
		m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		m_captureDATA.bFlip = FALSE;
		m_captureDATA.MapSize = sizeof(SharedTextureDATA);
		HookDATA* sharedCapture = m_dx.GetHookDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA();
		sharedTexture->TextureHandle = m_hTextureHandle;
		m_dx.m_ready.SetEvent();
		return TRUE;
	}
}
