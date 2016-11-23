#include "stdafx.h"
#include "DX10Capture.h"

namespace DXCapture
{
	DX10Capture::DX10Capture()
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_hTextureHandle(NULL),
		m_bDetour(FALSE),
		m_bCapturing(FALSE),
		m_bTextures(FALSE),
		m_hhk(NULL)
	{
	}

	DX10Capture::~DX10Capture()
	{
		if (m_hhk != NULL)
		{
			UnhookWindowsHookEx(m_hhk);
			m_hhk = NULL;
		}
	}
	DX10Capture& DX10Capture::Instance()
	{
		static DX10Capture instance;
		return instance;
	}
	BOOL DX10Capture::Initialize(HWND hWND)
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
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d10.dll"));
		m_d3d10.Reset(szD3DPath);
		if (!m_d3d10.IsValid())
			return FALSE;
		D3D10CREATEPROC d3d10Create = (D3D10CREATEPROC)m_d3d10.GetFunctionPointer(TEXT("D3D10CreateDeviceAndSwapChain"));
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
		TinyComPtr<IDXGISwapChain> swap;
		if (FAILED(hRes = (*d3d10Create)(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapDesc, &swap, &device)))
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
	BOOL DX10Capture::BuildEvents()
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
	void DX10Capture::Reset()
	{
		DX10Capture::Instance().m_bDetour = FALSE;
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_resource.Release();
		m_textureMemery.Unmap();
		m_textureMemery.Close();
	}
	BOOL DX10Capture::Setup(IDXGISwapChain *swap)
	{
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
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX10Capture::Instance().m_memery.Address();
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
		m_hhk = SetWindowsHookEx(WH_CBT, DX10Capture::CbtFilterHook, NULL, GetCurrentThreadId());
		return TRUE;
	}
	BOOL DX10Capture::Render(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX10Capture::Instance().m_memery.Address();
		if (!sharedCapture)
			return FALSE;
		if (m_bCapturing && m_stop.Lock(0))
		{
			m_bCapturing = TRUE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_start.Lock(0))
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
				}
			}
		}
		return TRUE;
	}
	BOOL DX10Capture::DX10GPUHook(ID3D10Device *device)
	{
		HRESULT hRes = S_OK;
		SharedCaptureDATA* sharedCapture = reinterpret_cast<SharedCaptureDATA*>(m_memery.Address());
		D3D10_TEXTURE2D_DESC texGameDesc;
		ZeroMemory(&texGameDesc, sizeof(texGameDesc));
		texGameDesc.Width = sharedCapture->Size.cx;
		texGameDesc.Height = sharedCapture->Size.cy;
		texGameDesc.MipLevels = 1;
		texGameDesc.ArraySize = 1;
		texGameDesc.Format = (DXGI_FORMAT)sharedCapture->Format;
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
	HRESULT STDMETHODCALLTYPE DX10Capture::DXGISwapPresent(IDXGISwapChain *pThis, UINT syncInterval, UINT flags)
	{
		DX10Capture::Instance().m_dxPresent.EndDetour();
		DX10Capture::Instance().m_lock.Lock();
		if (!DX10Capture::Instance().m_bDetour)
		{
			DX10Capture::Instance().m_bDetour = TRUE;
			DX10Capture::Instance().Setup(pThis);
		}
		if ((flags & DXGI_PRESENT_TEST) == 0)
		{
			DX10Capture::Instance().Render(pThis, flags);
		}
		DX10Capture::Instance().m_lock.Unlock();
		HRESULT hRes = pThis->Present(syncInterval, flags);
		DX10Capture::Instance().m_dxPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX10Capture::DXGISwapResizeBuffers(IDXGISwapChain *pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		DX10Capture::Instance().m_dxResizeBuffers.EndDetour();
		DX10Capture::Instance().m_lock.Lock();
		DX10Capture::Instance().Reset();
		DX10Capture::Instance().m_lock.Unlock();
		HRESULT hRes = pThis->ResizeBuffers(bufferCount, width, height, giFormat, flags);
		DX10Capture::Instance().m_dxResizeBuffers.BeginDetour();
		return hRes;
	}
	LRESULT CALLBACK DX10Capture::CbtFilterHook(INT code, WPARAM wParam, LPARAM lParam)
	{
		ASSERT(DX10Capture::Instance().m_hhk);
		if (code != HCBT_DESTROYWND)
			return CallNextHookEx(DX10Capture::Instance().m_hhk, code, wParam, lParam);
		SharedCaptureDATA* sharedCapture = (SharedCaptureDATA*)DX10Capture::Instance().m_memery.Address();
		if (sharedCapture->HwndCapture == reinterpret_cast<HWND>(wParam))
		{
			DX10Capture::Instance().Reset();
			DX10Capture::Instance().m_exit.SetEvent();
		}
		return CallNextHookEx(DX10Capture::Instance().m_hhk, code, wParam, lParam);
	}
}
