// DWMDetour64.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DWMCapture.h"

namespace DWM
{
	HRESULT WINAPI PresentDetour(IDXGISwapChainDWM *swap, UINT sync_interval, UINT flags)
	{

	}
	HRESULT WINAPI CreateSwapChainDetour(IDXGIFactoryDWM *factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChainDWM)
	{
		if (!ppFactory || !pDevice)
			return E_INVALIDARG;
		HRESULT hRes = g_dwmCapture.m_origCreateSwapChain(factory, pDevice, pDesc, pOutput, ppSwapChainDWM);
		if (FAILED(hRes))
			return hRes;

		return E_FAIL;
	}
	HRESULT WINAPI CreateDXGIFactoryDetour(REFIID iid, void **ppFactory)
	{
		if (!ppFactory)
			return E_INVALIDARG;
		if (!g_dwmCapture.m_origCreateDXGIFactory)
			return E_POINTER;
		HRESULT hRes = g_dwmCapture.m_origCreateDXGIFactory(iid, ppFactory);
		if (FAILED(hRes))
			return hRes;
		IUnknown* unknow = reinterpret_cast<IUnknown*>(*ppFactory);
		hRes = unknow->QueryInterface(__uuidof(IDXGIFactoryDWM), (void**)&g_dwmCapture.m_dxgiFactoryDWM);
		if (FAILED(hRes))
			return hRes;
		if (g_dwmCapture.m_createSwap.Initialize(g_dwmCapture.m_dxgiFactoryDWM->CreateSwapChain, CreateSwapChainDetour))
		{
			if (g_dwmCapture.m_createSwap.BeginDetour())
				return S_OK;
		}
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	DWMCapture::DWMCapture()
		:m_origCreateDXGIFactory(NULL),
		m_origCreateSwapChain(NULL)
	{

	}
	DWMCapture::~DWMCapture()
	{

	}

	BOOL DWMCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		return m_task.Submit(BindCallback(&DWMCapture::OnMessagePump, this));
	}

	BOOL DWMCapture::Detach(HMODULE hModule)
	{
		CHAR szName[MAX_PATH];
		memset(szName, 0, MAX_PATH);
		GetModuleBaseName(GetCurrentProcess(), NULL, szName, MAX_PATH);
		if (m_hWNDD3D != NULL)
		{
			::PostMessage(m_hWNDD3D, WM_CLOSE, NULL, NULL);
		}
		return m_task.Close(1500);
	}

	BOOL DWMCapture::BeginCapture()
	{
		TinyScopedLibrary lib("dxgi.dll");
		CreateDXGIFactory ps = reinterpret_cast<CreateDXGIFactory>(lib.GetFunctionPointer("CreateDXGIFactory"));
		if (m_createDXGIFactory.Initialize(ps, CreateDXGIFactoryDetour))
		{
			g_dwmCapture.m_origCreateDXGIFactory = reinterpret_cast<CreateDXGIFactory>(m_createDXGIFactory.GetOrig());
			return m_createDXGIFactory.BeginDetour();
		}
		return FALSE;
	}
	BOOL DWMCapture::EndCapture()
	{
		return TRUE;
	}
	void DWMCapture::OnMessagePump()
	{
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
			BeginCapture();
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			EndCapture();
		}
	}
	LRESULT CALLBACK DWMCapture::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			::DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam); ;
	}

}





