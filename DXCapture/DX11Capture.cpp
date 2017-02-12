#include "stdafx.h"
#include "DX11Capture.h"

namespace DXCapture
{
	DX11Capture::DX11Capture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_dx(dx)
	{
	}

	DX11Capture::~DX11Capture()
	{

	}
	BOOL DX11Capture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
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
		D3D_FEATURE_LEVEL level;
		if (FAILED(hRes = (*d3d11Create)(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, levels, 6, D3D11_SDK_VERSION, &swapDesc, &swap, &device, &level, &immediateContext)))
			return FALSE;
		LOG(INFO) << "DX11Capture::Initialize OK\n";
		return TRUE;
	}
	void DX11Capture::Release()
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_resource.Release();
		m_dx.m_textureMemery.Unmap();
		m_dx.m_textureMemery.Close();
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
		m_dxgiFormat = GetDX10PlusTextureFormat(scd.BufferDesc.Format);
		m_captureDATA.Format = m_dxgiFormat;
		m_captureDATA.Size.cx = scd.BufferDesc.Width;
		m_captureDATA.Size.cy = scd.BufferDesc.Height;
		m_captureDATA.HwndCapture = scd.OutputWindow;
		m_captureDATA.bMultisample = scd.SampleDesc.Count > 1;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX11Capture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.Lock(0))
		{
			LOG(INFO) << "DX11Capture::Render m_stop - OK\n";
			m_bCapturing = FALSE;
			Release();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.Lock(0))
		{
			LOG(INFO) << "DX11Capture::Render m_start - OK\n";
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D11Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bTextures)
			{
				m_bTextures = DX11GPUHook(device);
				LOG(INFO) << "DX11Capture::Render DX11GPUHook\n";
				LOG(INFO) << "DX11Capture::Render  swap->GetDevice m_bCapturing:" << m_bCapturing << "m_bTextures:" << m_bTextures << "\n";
				LOG(INFO) << "------------------------------------------------------------------------------------------------\n";
			}
			if (m_bTextures)
			{
				TinyComPtr<ID3D11DeviceContext> context;
				device->GetImmediateContext(&context);
				TinyComPtr<ID3D11Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
				{
					if (m_captureDATA.bMultisample)
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
		D3D11_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = m_captureDATA.Size.cx;
		texGameDesc.Height = m_captureDATA.Size.cy;
		texGameDesc.MipLevels = 1;
		texGameDesc.ArraySize = 1;
		texGameDesc.Format = m_dxgiFormat;
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
		m_captureDATA.CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		m_captureDATA.bFlip = FALSE;
		m_captureDATA.MapSize = sizeof(SharedTextureDATA);
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA();
		sharedTexture->TextureHandle = m_hTextureHandle;
		m_dx.m_ready.SetEvent();
		return TRUE;
	}
	/*BOOL DX11Capture::SaveAs(ID3D11DeviceContext* context, D3DX11_IMAGE_FILE_FORMAT s, LPCSTR pzFile)
	{
		ASSERT(m_resource && context);
		return D3DX11SaveTextureToFile(context, m_resource, s, pzFile);
	}*/
}
