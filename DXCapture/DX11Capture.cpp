#include "stdafx.h"
#include "DX11Capture.h"

namespace DXCapture
{
	DX11Capture::DX11Capture()
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_hhk(NULL)
	{
	}

	DX11Capture::~DX11Capture()
	{
		if (m_hhk != NULL)
		{
			UnhookWindowsHookEx(m_hhk);
			m_hhk = NULL;
		}
	}
	DX11Capture& DX11Capture::Instance()
	{
		static DX11Capture instance;
		return instance;
	}
	BOOL DX11Capture::Initialize(HWND hWND)
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
	BOOL DX11Capture::BuildEvents()
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
	void DX11Capture::Reset()
	{
		DX11Capture::Instance().m_bDetour = FALSE;
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_resource.Release();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
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
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX11Capture::Instance().m_memery.Address();
		sharedCapture->Format = (DWORD)scd.BufferDesc.Format;
		sharedCapture->Size.cx = scd.BufferDesc.Width;
		sharedCapture->Size.cy = scd.BufferDesc.Height;
		sharedCapture->HwndCapture = scd.OutputWindow;
		sharedCapture->bMultisample = scd.SampleDesc.Count > 1;
		m_dxgiFormat = scd.BufferDesc.Format;
		if (m_hhk != NULL)
		{
			UnhookWindowsHookEx(m_hhk);
			m_hhk = NULL;
		}
		m_hhk = SetWindowsHookEx(WH_CBT, DX11Capture::CbtFilterHook, NULL, GetCurrentThreadId());
		return TRUE;
	}
	BOOL DX11Capture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX11Capture::Instance().m_memery.Address();
		if (!sharedCapture)
			return FALSE;
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
	/*BOOL DX11Capture::SaveBitmap(LPCSTR pzFile)
	{
		if (SharedTextureDATA* sharedTexture = (SharedTextureDATA*)m_textureMemery.Address())
		{
			if (sharedTexture->TextureHandle)
			{
				TinyComPtr<ID3D10Resource> resource;
				HRESULT hRes = m_device->OpenSharedResource(sharedTexture->TextureHandle, __uuidof(ID3D10Resource), (void**)&resource);
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
				hRes = m_device->CreateTexture2D(&desc1, NULL, &d3d10Texture2D);
				if (FAILED(hRes))
				{
					return FALSE;
				}
				m_device->CopyResource(d3d10Texture2D, resource);

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
	BOOL DX11Capture::DX11GPUHook(ID3D11Device *device)
	{
		/*PFN_D3D10_CREATE_DEVICE1 d3d10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1)m_d3d10_1.GetFunctionPointer(TEXT("D3D10CreateDevice1"));
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
		if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_device)))
		{
			if (FAILED((*d3d10CreateDevice1)(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_device)))
			{
				return FALSE;
			}
		}*/

		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = reinterpret_cast<SharedCaptureDATA*>(m_memery.Address());
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
		if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
		{
			if (!m_textureMemery.Create(TEXTURE_MEMORY, sizeof(SharedTextureDATA)))
				return FALSE;
		}
		m_textureMemery.Unmap();
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
	LRESULT CALLBACK DX11Capture::CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam)
	{
		ASSERT(DX11Capture::Instance().m_hhk);
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(DX11Capture::Instance().m_hhk, code, wParam, lParam);
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX11Capture::Instance().m_memery.Address();
		if (sharedCapture->HwndCapture == reinterpret_cast<HWND>(wParam))
		{
			DX11Capture::Instance().Reset();
			DX11Capture::Instance().m_exit.SetEvent();
		}
		return CallNextHookEx(DX11Capture::Instance().m_hhk, code, wParam, lParam);
	}
}
