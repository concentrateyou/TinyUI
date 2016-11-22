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
		m_d3d11.Reset(szD3DPath);
		if (!m_d3d11.IsValid())
			return FALSE;
		D3D11CREATEPROC d3d11Create = (D3D11CREATEPROC)m_d3d11.GetFunctionPointer(TEXT("D3D11CreateDeviceAndSwapChain"));
		if (!d3d11Create)
			return FALSE;
		TinyComPtr<ID3D11Device> d3d11;
		D3D_FEATURE_LEVEL desiredLevels[6] =
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
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width = 2;
		swapDesc.BufferDesc.Height = 2;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = hWND;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.Windowed = TRUE;
		TinyComPtr<IUnknown> context;
		TinyComPtr<IUnknown> device;
		D3D_FEATURE_LEVEL receivedLevel;
		if (FAILED(hRes = (*d3d11Create)(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, desiredLevels, 6, D3D11_SDK_VERSION, &swapDesc, &m_swap, &device, &receivedLevel, &context)))
			return FALSE;
		ULONG *vtable = *(ULONG**)m_swap.Ptr();
		m_dxPresent.Initialize((FARPROC)*(vtable + (32 / 4)), (FARPROC)DXGISwapPresent);
		m_dxResizeBuffers.Initialize((FARPROC)*(vtable + (52 / 4)), (FARPROC)DXGISwapResizeBuffers);
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
		m_bTextures = FALSE;
		m_hTextureHandle = NULL;
		m_textureMemery.Unmap();
		m_textureMemery.Close();
	}
	HRESULT STDMETHODCALLTYPE DX11Capture::DXGISwapPresent(IDXGISwapChain *pThis, UINT syncInterval, UINT flags)
	{
		DX11Capture::Instance().m_dxPresent.EndDetour();
		HRESULT hRes = S_OK;
		DX11Capture::Instance().m_lock.Lock();
		DX11Capture::Instance().m_lock.Unlock();
		hRes = pThis->Present(syncInterval, flags);
		DX11Capture::Instance().m_dxPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX11Capture::DXGISwapResizeBuffers(IDXGISwapChain *pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		DX11Capture::Instance().m_dxResizeBuffers.EndDetour();
		HRESULT hRes = S_OK;
		DX11Capture::Instance().m_lock.Lock();
		DX11Capture::Instance().m_lock.Unlock();
		hRes = pThis->ResizeBuffers(bufferCount, width, height, giFormat, flags);
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
