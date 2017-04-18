// DWMDetour64.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DWMCapture.h"
#include <D3DX10tex.h>
#pragma comment(lib,"D3DX10.lib")

namespace DWM
{
	typedef HRESULT(WINAPI *D3D101CREATEPROC)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device1**);

	DXGI_FORMAT GetDWMPlusTextureFormat(DXGI_FORMAT s)
	{
		switch (s)
		{
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		return s;
	}

	HRESULT STDMETHODCALLTYPE DWM_DXGISwapPresent(IDXGISwapChain *pThis, UINT syncInterval, UINT flags)
	{
		LOG(INFO) << "DWM_DXGISwapPresent\n";
		g_dwm.m_dxPresent.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(pThis->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dwm.m_bDX101 = TRUE;
					if (g_dwm.m_currentSwap == NULL)
					{
						g_dwm.m_currentSwap = g_dwm.Setup(pThis) ? pThis : NULL;
						LOG(INFO) << "DWMCapture::m_currentSwap:" << g_dwm.m_currentSwap << "\n";
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dwm.m_currentSwap == pThis)
					{
						g_dwm.Render(pThis, flags);
						LOG(INFO) << "DWMCapture::Render\n";
					}
					break;
				}
			} while (0);
		}
		HRESULT hRes = pThis->Present(syncInterval, flags);
		g_dwm.m_dxPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DWM_DXGISwapResizeBuffers(IDXGISwapChain *pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		LOG(INFO) << "DWM_DXGISwapResizeBuffers\n";
		g_dwm.m_dxResizeBuffers.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(pThis->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dwm.m_bDX101 = TRUE;
					g_dwm.Release();
					break;
				}
			} while (0);
		}
		g_dwm.m_currentSwap = NULL;
		HRESULT hRes = pThis->ResizeBuffers(bufferCount, width, height, giFormat, flags);
		g_dwm.m_dxResizeBuffers.BeginDetour();
		return hRes;
	}

	DWMCapture::DWMCapture()
		:m_currentSwap(NULL),
		m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_hD3D101(NULL)
	{

	}
	DWMCapture::~DWMCapture()
	{

	}

	BOOL DWMCapture::Attach(HMODULE hModule)
	{
		TinyUI::SetLogFile("D:\\DWM.log");
		m_hInstance = hModule;
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		return m_task.Submit(BindCallback(&DWMCapture::OnMessagePump, this));
	}

	BOOL DWMCapture::Detach(HMODULE hModule)
	{
		LOG(INFO) << "DWMCapture::Detach\n";
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		if (m_hWNDD3D != NULL)
		{
			::PostMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
		}
		return m_task.Close(1500);
	}
	BOOL DWMCapture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device1> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D10Device1), (void**)&device)))
		{
			if (!m_bTextures)
			{
				LOG(INFO) << "DWMCapture::DX101GPUHook\n";
				m_bTextures = DX101GPUHook(device);
			}
			if (m_bTextures)
			{
				LOG(INFO) << "DWMCapture::CopyResource\n";
				TinyComPtr<ID3D10Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D10Resource), (void**)&backBuffer)))
				{
					if (m_captureDATA.bMultisample)
					{
						device->ResolveSubresource(m_resource, 0, backBuffer, 0, m_dxgiFormat);
					}
					else
					{
						device->CopyResource(m_resource, backBuffer);
					}
					D3DX10SaveTextureToFile(m_resource, D3DX10_IFF_PNG, "D:\\desktopp.png");
				}
			}
		}
		return TRUE;
	}
	BOOL DWMCapture::Setup(IDXGISwapChain *swap)
	{
		LOG(INFO) << "DX101Capture Setup OK\n";
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device1> device;
		hRes = swap->GetDevice(__uuidof(ID3D10Device1), (void**)&device);
		if (hRes != S_OK)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (hRes != S_OK)
			return FALSE;
		m_dxgiFormat = GetDWMPlusTextureFormat(scd.BufferDesc.Format);
		m_captureDATA.Format = m_dxgiFormat;
		m_captureDATA.Size.cx = scd.BufferDesc.Width;
		m_captureDATA.Size.cy = scd.BufferDesc.Height;
		m_captureDATA.HwndCapture = scd.OutputWindow;
		m_captureDATA.bMultisample = scd.SampleDesc.Count > 1;
		return TRUE;
	}
	void DWMCapture::Release()
	{
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_resource.Release();
	}
	BOOL DWMCapture::DX101GPUHook(ID3D10Device1 *device)
	{
		HRESULT hRes = S_OK;
		D3D10_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = m_captureDATA.Size.cx;
		texGameDesc.Height = m_captureDATA.Size.cy;
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
		m_captureDATA.CaptureType = 2;
		m_captureDATA.bFlip = FALSE;
		m_captureDATA.MapSize = sizeof(DWMCaptureDATA);
		return TRUE;
	}
	BOOL DWMCapture::BeginCapture()
	{
		LOG(INFO) << "DWMCapture::BeginCapture\n";
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d10_1.dll"));
		m_hD3D101 = GetModuleHandle(szD3DPath);
		if (m_hD3D101 == NULL)
			return FALSE;
		D3D101CREATEPROC d3d101Create = (D3D101CREATEPROC)GetProcAddress(m_hD3D101, TEXT("D3D10CreateDeviceAndSwapChain1"));
		if (!d3d101Create)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 2;
		swapDesc.BufferDesc.Width = 1;
		swapDesc.BufferDesc.Height = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = m_hWNDD3D;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.Windowed = TRUE;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0;
		TinyComPtr<ID3D10Device1>	device;
		TinyComPtr<IDXGISwapChain>	swap;
		if (FAILED(hRes = (*d3d101Create)(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &swapDesc, &swap, &device)))
			return FALSE;
		LOG(INFO) << "DX101Capture::Initialize OK\n";
		ULONG *vtable = *(ULONG**)swap.Ptr();
		if (!m_dxPresent.Initialize((FARPROC)*(vtable + (32 / 4)), (FARPROC)DWM_DXGISwapPresent))
		{
			LOG(INFO) << "m_dxPresent::Initialize FAIL\n";
			return FALSE;
		}
		if (!m_dxResizeBuffers.Initialize((FARPROC)*(vtable + (52 / 4)), (FARPROC)DWM_DXGISwapResizeBuffers))
		{
			LOG(INFO) << "m_dxResizeBuffers::Initialize FAIL\n";
			return FALSE;
		}
		if (!m_dxPresent.BeginDetour())
		{
			LOG(INFO) << "m_dxPresent::BeginDetour FAIL\n";
			return FALSE;
		}
		if (!m_dxResizeBuffers.BeginDetour())
		{
			LOG(INFO) << "m_dxResizeBuffers::BeginDetour FAIL\n";
			return FALSE;
		}
		return TRUE;
	}
	BOOL DWMCapture::EndCapture()
	{
		LOG(INFO) << "DWMCapture::EndCapture\n";
		return TRUE;
	}
	void DWMCapture::OnMessagePump()
	{
		LOG(INFO) << "DWMCapture::OnMessagePump\n";
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = (WNDPROC)DWMCapture::WindowProcedure;
		wc.lpszClassName = DWM_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			m_hWNDD3D = CreateWindowEx(0,
				DWM_WINDOWCLASS,
				TEXT("DWM Caption Window"),
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				m_hInstance,
				NULL
			);
		}
		if (m_hWNDD3D != NULL)
		{
			LOG(INFO) << "CreateWindowEx OK\n";
			BeginCapture();
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			EndCapture();
		}
		else
		{
			LOG(INFO) << "CreateWindowEx FAIL\n";
		}
	}
	LRESULT CALLBACK DWMCapture::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			LOG(INFO) << "WM_CLOSE\n";
			::DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			LOG(INFO) << "WM_DESTROY\n";
			::PostQuitMessage(0);
			break;
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam); ;
	}
}





