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
	BOOL DX2D::Initialize(HWND hWND, INT x, INT y, INT cx, INT cy)
	{
		m_hWND = hWND;
		m_pos.x = x;
		m_pos.y = y;
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
		DXGI_SWAP_CHAIN_DESC1 props = {};
		props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		props.SampleDesc.Count = 1;
		props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		props.BufferCount = 2;
		hRes = factory->CreateSwapChainForHwnd(m_d3d, hWND, &props, NULL, NULL, &m_swap);
		if (FAILED(hRes))
			return FALSE;
		hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), reinterpret_cast<void**>(&m_factory));
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID2D1Device> d2d1device;
		hRes = m_factory->CreateDevice(dxgi, &d2d1device);
		if (FAILED(hRes))
			return FALSE;
		hRes = d2d1device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_target);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGISurface> surface;
		hRes = m_swap->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
		if (FAILED(hRes))
			return FALSE;
		D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		TinyComPtr<ID2D1Bitmap1> bitmap;
		hRes = m_target->CreateBitmapFromDxgiSurface(surface, (const D2D1_BITMAP_PROPERTIES1*)&props, &bitmap);
		if (FAILED(hRes))
			return FALSE;
		m_target->SetTarget(bitmap);
		FLOAT dpiX, dpiY;
		m_factory->GetDesktopDpi(&dpiX, &dpiY);
		m_target->SetDpi(dpiX, dpiY);
		return TRUE;
	}
}

