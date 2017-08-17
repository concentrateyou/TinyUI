#include "stdafx.h"
#include "DX2D.h"

namespace DXFramework
{
	DX2D::DX2D()
	{
		//SetLogFile("D:\\dx.log");
	}

	DX2D::~DX2D()
	{

	}
	BOOL DX2D::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		DWORD dwFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		dwFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT hRes = S_OK;
		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_9_3;
		hRes = D3D11CreateDevice(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			dwFlag,
			levels,
			sizeof(levels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,
			&m_d3d,
			&level,
			NULL);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D D3D11CreateDevice Fail\n";
			return FALSE;
		}
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = m_d3d->QueryInterface(&dxgi);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D QueryInterface IDXGIDevice Fail\n";
			return FALSE;
		}
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D GetAdapter IDXGIAdapter Fail\n";
			return FALSE;
		}
		TinyComPtr<IDXGIFactory2> factory;
		hRes = adapter->GetParent(__uuidof(factory), reinterpret_cast<void **>(&factory));
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D GetParent IDXGIFactory2 Fail\n";
			return FALSE;
		}
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc = { 0 };
		desc.RefreshRate.Numerator = 60;
		desc.RefreshRate.Denominator = 1;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.Windowed = TRUE;
		DXGI_SWAP_CHAIN_DESC1 dest1 = {};
		dest1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		dest1.SampleDesc.Count = 2;
		dest1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dest1.BufferCount = 2;
		dest1.Width = cx;
		dest1.Height = cy;
		hRes = factory->CreateSwapChainForHwnd(m_d3d, hWND, &dest1, &desc, NULL, &m_swap);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D CreateSwapChainForHwnd Fail\n";
			return FALSE;
		}
		hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), reinterpret_cast<void**>(&m_factory));
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D D2D1CreateFactory Fail\n";
			return FALSE;
		}
		hRes = m_factory->QueryInterface(&m_d2dMultithread);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D QueryInterface ID2D1Multithread Fail\n";
			return FALSE;
		}
		TinyComPtr<ID2D1Device> d2d1device;
		hRes = m_factory->CreateDevice(dxgi, &d2d1device);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D CreateDevice ID2D1Device Fail\n";
			return FALSE;
		}
		hRes = d2d1device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_context);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D CreateDeviceContext Fail\n";
			return FALSE;
		}
		TinyComPtr<IDXGISurface> surface;
		hRes = m_swap->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D GetBuffer Fail\n";
			return FALSE;
		}
		m_bitmap.Release();
		D2D1_BITMAP_PROPERTIES1 props1 = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		hRes = m_context->CreateBitmapFromDxgiSurface(surface, (const D2D1_BITMAP_PROPERTIES1*)&props1, &m_bitmap);
		if (hRes != S_OK)
		{
			//LOG(INFO) << "DX2D CreateBitmapFromDxgiSurface Fail\n";
			return FALSE;
		}
		m_context->SetTarget(m_bitmap);
		FLOAT dpiX, dpiY;
		m_factory->GetDesktopDpi(&dpiX, &dpiY);
		m_context->SetDpi(dpiX, dpiY);
		//LOG(INFO) << "DX2D Initialize OK\n";
		return TRUE;
	}

	BOOL DX2D::Enter()
	{
		if (!m_d2dMultithread)
			return FALSE;
		if (m_d2dMultithread->GetMultithreadProtected())
		{
			m_d2dMultithread->Enter();
		}
		return TRUE;
	}
	BOOL DX2D::Leave()
	{
		if (!m_d2dMultithread)
			return FALSE;
		if (m_d2dMultithread->GetMultithreadProtected())
		{
			m_d2dMultithread->Leave();
		}
		return TRUE;
	}

	BOOL DX2D::BeginDraw(ID2D1Bitmap1* bitmap)
	{
		if (m_context != NULL)
		{
			m_context->SetTarget(bitmap != NULL ? bitmap : m_bitmap);
			m_context->BeginDraw();
			m_context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX2D::EndDraw()
	{
		if (m_context != NULL)
		{
			m_context->EndDraw();
			return SUCCEEDED(m_swap->Present(0, 0));
		}
		return FALSE;
	}
	BOOL DX2D::Resize()
	{
		if (!m_context)
			return FALSE;
		m_context->SetTarget(NULL);
		HRESULT hRes = m_swap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IDXGISurface> surface;
		hRes = m_swap->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
		if (hRes != S_OK)
			return FALSE;
		m_bitmap.Release();
		D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		hRes = m_context->CreateBitmapFromDxgiSurface(surface, (const D2D1_BITMAP_PROPERTIES1*)&props, &m_bitmap);
		if (hRes != S_OK)
			return FALSE;
		m_context->SetTarget(m_bitmap);
		FLOAT dpiX, dpiY;
		m_factory->GetDesktopDpi(&dpiX, &dpiY);
		m_context->SetDpi(dpiX, dpiY);
		return TRUE;
	}

	ID2D1DeviceContext* DX2D::GetContext() const
	{
		return m_context;
	}
	HWND DX2D::GetHWND() const
	{
		return m_hWND;
	}
}

