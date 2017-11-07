#include "stdafx.h"
#include "DX2D.h"

namespace DXFramework
{
	DX2D::DX2D()
		:m_hWND(NULL)
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
		D2D1_FACTORY_OPTIONS opts;
		ZeroMemory(&opts, sizeof(D2D1_FACTORY_OPTIONS));
		opts.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
		HRESULT hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &opts, reinterpret_cast<void**>(&m_factory));
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D D2D1CreateFactory Fail\n";
			return FALSE;
		}
		return Create(hWND, cx, cy);
	}
	BOOL DX2D::Create(HWND hWND, INT cx, INT cy)
	{
		m_hwndRenderTarget.Release();
		D2D1_PIXEL_FORMAT pixelFormat;
		pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = {
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			pixelFormat,
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		};
		D2D1_SIZE_U size = D2D1::SizeU(cx, cy);
		D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTargetProperties = {
			hWND,
			size,
			D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS
		};
		HRESULT hRes = m_factory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTargetProperties, &m_hwndRenderTarget);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D CreateHwndRenderTarget Fail\n";
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX2D::BeginDraw()
	{
		if (m_hwndRenderTarget != NULL)
		{
			if (m_hwndRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
			{
				return FALSE;
			}
			m_hwndRenderTarget->BeginDraw();
			m_hwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_hwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX2D::EndDraw(HRESULT& hRes)
	{
		if (m_hwndRenderTarget != NULL)
		{
			hRes = m_hwndRenderTarget->EndDraw();
			if (hRes == D2DERR_RECREATE_TARGET)
			{
				LOG(ERROR) << "[MPreviewController] " << "EndDraw D2DERR_RECREATE_TARGET";
				return Create(m_hWND, m_size.cx, m_size.cy);
			}
		}
		return hRes == S_OK;
	}
	BOOL DX2D::Resize(INT cx, INT cy)
	{
		if (m_hwndRenderTarget != NULL)
		{
			D2D1_SIZE_U size;
			size.width = cx;
			size.height = cy;
			return SUCCEEDED(m_hwndRenderTarget->Resize(size));
		}
		return FALSE;
	}
	ID2D1HwndRenderTarget* DX2D::GetCanvas() const
	{
		return m_hwndRenderTarget;
	}
	HWND DX2D::GetHWND() const
	{
		return m_hWND;
	}
}

