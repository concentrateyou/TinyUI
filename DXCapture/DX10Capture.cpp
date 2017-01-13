#include "stdafx.h"
#include "DX10Capture.h"

namespace DXCapture
{
	DX10Capture::DX10Capture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_hD3D10(NULL),
		m_dx(dx)
	{
	}

	DX10Capture::~DX10Capture()
	{

	}
	BOOL DX10Capture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
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
	void DX10Capture::Reset(BOOL bRelease)
	{
		m_bDetour = FALSE;
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		if (bRelease)
		{
			m_resource.Release();
		}
		else
		{
			m_resource.Detach();
		}
	}
	BOOL DX10Capture::Setup(IDXGISwapChain *swap)
	{
		LOG(INFO) << "DX10Capture Setup OK\n";
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device);
		if (FAILED(hRes))
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (FAILED(hRes))
			return FALSE;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		m_dxgiFormat = GetDX10PlusTextureFormat(scd.BufferDesc.Format);
		sharedCapture->Format = (DWORD)m_dxgiFormat;
		sharedCapture->Size.cx = scd.BufferDesc.Width;
		sharedCapture->Size.cy = scd.BufferDesc.Height;
		sharedCapture->HwndCapture = scd.OutputWindow;
		sharedCapture->bMultisample = scd.SampleDesc.Count > 1;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX10Capture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		if (m_bCapturing && m_dx.m_stop.Lock(0))
		{
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.Lock(0))
		{
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D10Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bTextures)
			{
				m_bTextures = DX10GPUHook(device);
			}
			if (m_bTextures)
			{
				TinyComPtr<ID3D10Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D10Resource), (void**)&backBuffer)))
				{
					if (sharedCapture->bMultisample)
					{
						device->ResolveSubresource(m_resource, 0, backBuffer, 0, m_dxgiFormat);
					}
					else
					{
						device->CopyResource(m_resource, backBuffer);
					}
					//SaveAs(D3DX10_IFF_DDS, "D:\\123.dds");
				}
			}
		}
		return TRUE;
	}
	BOOL DX10Capture::DX10GPUHook(ID3D10Device *device)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		ASSERT(sharedCapture);
		D3D10_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = sharedCapture->Size.cx;
		texGameDesc.Height = sharedCapture->Size.cy;
		texGameDesc.MipLevels = 1;
		texGameDesc.ArraySize = 1;
		texGameDesc.Format = m_dxgiFormat;
		texGameDesc.SampleDesc.Count = 1;
		texGameDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		texGameDesc.Usage = D3D10_USAGE_DEFAULT;
		texGameDesc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D10Texture2D> dx10Texture2D;
		if (FAILED(hRes = device->CreateTexture2D(&texGameDesc, NULL, &dx10Texture2D)))
			return FALSE;
		if (FAILED(hRes = dx10Texture2D->QueryInterface(__uuidof(ID3D10Resource), (void**)&m_resource)))
			return FALSE;
		TinyComPtr<IDXGIResource> resource;
		if (FAILED(hRes = dx10Texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->GetSharedHandle(&m_hTextureHandle)))
			return FALSE;
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA();
		ASSERT(sharedTexture);
		sharedTexture->TextureHandle = m_hTextureHandle;
		sharedCapture->CaptureType = CAPTURETYPE_SHAREDTEX;
		sharedCapture->bFlip = FALSE;
		m_dx.m_ready.SetEvent();
		return TRUE;
	}
	BOOL DX10Capture::SaveAs(D3DX10_IMAGE_FILE_FORMAT s, LPCSTR pzFile)
	{
		ASSERT(m_resource);
		return D3DX10SaveTextureToFile(m_resource, s, pzFile);
	}
}
