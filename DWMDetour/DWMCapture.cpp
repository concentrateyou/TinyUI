// DWMDetour64.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DWMCapture.h"

namespace DWM
{
	//HRESULT WINAPI PresentDetour(IDXGISwapChainDWM *swap, UINT sync_interval, UINT flags)
	//{
	//	return S_OK;
	//}
	//HRESULT WINAPI CreateSwapChainDetour(IDXGIFactoryDWM *factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChainDWM)
	//{
	//	LOG(INFO) << "CreateSwapChainDetour OK\n";
	//	if (!factory || !pDevice)
	//		return E_INVALIDARG;
	//	HRESULT hRes = g_dwmCapture.m_origCreateSwapChain(factory, pDevice, pDesc, pOutput, ppSwapChainDWM);
	//	if (FAILED(hRes))
	//		return hRes;

	//	return E_FAIL;
	//}
	//HRESULT WINAPI CreateDXGIFactoryDetour(REFIID iid, void **ppFactory)
	//{
	//	LOG(INFO) << "CreateDXGIFactoryDetour OK\n";
	//	if (!ppFactory)
	//		return E_INVALIDARG;
	//	if (!g_dwmCapture.m_origCreateDXGIFactory)
	//		return E_POINTER;
	//	HRESULT hRes = g_dwmCapture.m_origCreateDXGIFactory(iid, ppFactory);
	//	if (FAILED(hRes))
	//		return hRes;
	//	IUnknown* unknow = reinterpret_cast<IUnknown*>(*ppFactory);
	//	hRes = unknow->QueryInterface(__uuidof(IDXGIFactoryDWM), (void**)&g_dwmCapture.m_dxgiFactoryDWM);
	//	if (FAILED(hRes))
	//		return hRes;
	//	ULONG *vtable = *(ULONG**)g_dwmCapture.m_dxgiFactoryDWM.Ptr();
	//	if (g_dwmCapture.m_createSwap.Initialize((FARPROC)*(vtable + 3), CreateSwapChainDetour))
	//	{
	//		if (g_dwmCapture.m_createSwap.BeginDetour())
	//			return S_OK;
	//	}
	//	return E_FAIL;
	//}
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

	BOOL DWMCapture::BeginCapture()
	{
		LOG(INFO) << "DWMCapture::BeginCapture\n";
		TinyComPtr<ID3D10Device1>		d3d101;
		TinyComPtr<IDXGIFactory>		dxgiFactory;
		TinyComPtr<IDXGIFactoryDWM>		dxgiFactoryDWM;
		TinyComPtr<IDXGISwapChainDWM>	dxgiSwapDWM;
		TinyComPtr<IDXGIDevice>			dxgi;
		TinyComPtr<IDXGIAdapter>		dxgiAdapter;
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
		HRESULT hRes = D3D10CreateDevice1(NULL,
			D3D10_DRIVER_TYPE_NULL,
			NULL,
			0,
			D3D10_FEATURE_LEVEL_10_1,
			D3D10_1_SDK_VERSION,
			&d3d101);
		if (FAILED(hRes))
		{
			LOG(INFO) << "D3D10CreateDevice1 FAIL\n";
			return FALSE;
		}
		else
		{
			LOG(INFO) << "D3D10CreateDevice1 OK\n";
			hRes = d3d101->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi);
			if (FAILED(hRes))
			{
				LOG(INFO) << "QueryInterface IDXGIDevice FAIL\n";
				return FALSE;
			}
			LOG(INFO) << "QueryInterface IDXGIDevice OK\n";
			hRes = dxgi->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgiAdapter);
			if (FAILED(hRes))
			{
				LOG(INFO) << "GetParent IDXGIAdapter FAIL\n";
				return FALSE;
			}
			LOG(INFO) << "GetParent IDXGIAdapter OK\n";
			hRes = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
			if (FAILED(hRes))
			{
				LOG(INFO) << "GetParent IDXGIFactory FAIL\n";
				return FALSE;
			}
			LOG(INFO) << "GetParent IDXGIFactory OK\n";
			hRes = dxgiFactory->QueryInterface(__uuidof(IDXGIFactoryDWM), (void **)&dxgiFactoryDWM);
			if (FAILED(hRes))
			{
				LOG(INFO) << "QueryInterface IDXGIFactoryDWM FAIL\n";
				return FALSE;
			}
			LOG(INFO) << "QueryInterface IDXGIFactoryDWM OK\n";
			hRes = dxgiFactoryDWM->CreateSwapChain(d3d101, &swapDesc, NULL, &dxgiSwapDWM);
			if (FAILED(hRes))
			{
				if (hRes == DXGI_ERROR_INVALID_CALL)
				{
					LOG(INFO) << "CreateSwapChainDWM FAIL DXGI_ERROR_INVALID_CALL\n:";
				}
				else
				{
					LOG(INFO) << "CreateSwapChainDWM FAIL:" << hRes << "\n";
				}
				return FALSE;
			}
			LOG(INFO) << "CreateSwapChainDWM OK\n";
			return TRUE;
		}
		/*TinyScopedLibrary lib("dxgi.dll");
		CreateDXGIFactory  ps = reinterpret_cast<CreateDXGIFactory>(lib.GetFunctionPointer("CreateDXGIFactory"));
		HRESULT hRes = ps(__uuidof(IDXGIFactoryDWM), (void**)&m_dxgiFactoryDWM);
		if (SUCCEEDED(hRes))
		{
			LOG(INFO) << "Create IDXGIFactoryDWM OK\n";
		}
		else
		{
			LOG(INFO) << "Create IDXGIFactoryDWM FAIL\n";
		}*/
		return FALSE;
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





