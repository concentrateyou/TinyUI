#include "stdafx.h"
#include "DX2D.h"

namespace DXFramework
{
	DX2D::DX2D()
	{
	}

	DX2D::~DX2D()
	{
	}
	BOOL DX2D::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_size.cx = cx;
		m_size.cy = cy;
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
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = m_d3d->QueryInterface(&dxgi);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIFactory2> factory;
		hRes = adapter->GetParent(__uuidof(factory), reinterpret_cast<void **>(&factory));
		if (FAILED(hRes))
			return FALSE;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc = { 0 };
		desc.RefreshRate.Numerator = 60;
		desc.RefreshRate.Denominator = 1;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.Windowed = FALSE;
		DXGI_SWAP_CHAIN_DESC1 props = {};
		props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		props.SampleDesc.Count = 2;
		props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		props.BufferCount = 2;
		props.Width = m_size.cx;
		props.Height = m_size.cy;
		hRes = factory->CreateSwapChainForHwnd(m_d3d, hWND, &props, &desc, NULL, &m_swap);
		if (FAILED(hRes))
			return FALSE;
		hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), reinterpret_cast<void**>(&m_factory));
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID2D1Device> d2d1device;
		hRes = m_factory->CreateDevice(dxgi, &d2d1device);
		if (FAILED(hRes))
			return FALSE;
		hRes = d2d1device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_context);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGISurface> surface;
		hRes = m_swap->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
		if (FAILED(hRes))
			return FALSE;
		D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		TinyComPtr<ID2D1Bitmap1> bitmap;
		hRes = m_context->CreateBitmapFromDxgiSurface(surface, (const D2D1_BITMAP_PROPERTIES1*)&props, &bitmap);
		if (FAILED(hRes))
			return FALSE;
		m_context->SetTarget(bitmap);
		FLOAT dpiX, dpiY;
		m_factory->GetDesktopDpi(&dpiX, &dpiY);
		m_context->SetDpi(dpiX, dpiY);
		return TRUE;
	}
	BOOL DX2D::BeginDraw()
	{
		ASSERT(m_context);
		m_context->BeginDraw();
		return TRUE;
	}
	BOOL DX2D::EndDraw()
	{
		ASSERT(m_context);
		m_context->EndDraw();
		return SUCCEEDED(m_swap->Present(0, 0));
	}
	BOOL DX2D::Resize()
	{
		ASSERT(m_context);
		m_context->SetTarget(NULL);
		HRESULT hRes = m_swap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGISurface> surface;
		hRes = m_swap->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
		if (FAILED(hRes))
			return FALSE;
		D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		TinyComPtr<ID2D1Bitmap1> bitmap;
		hRes = m_context->CreateBitmapFromDxgiSurface(surface, (const D2D1_BITMAP_PROPERTIES1*)&props, &bitmap);
		if (FAILED(hRes))
			return FALSE;
		m_context->SetTarget(bitmap);
		FLOAT dpiX, dpiY;
		m_factory->GetDesktopDpi(&dpiX, &dpiY);
		m_context->SetDpi(dpiX, dpiY);
		return TRUE;
	}
	ID2D1DeviceContext* DX2D::GetContext() const
	{
		return m_context;
	}
}

