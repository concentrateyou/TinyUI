#include "stdafx.h"
#include "DX2D.h"

namespace DXFramework
{
	DX2D::DX2D()
		:m_hWND(NULL),
		m_hDC(NULL)
	{
	}

	DX2D::~DX2D()
	{

	}
	BOOL DX2D::Initialize(HWND hWND, INT cx, INT cy)
	{
		m_hWND = hWND;
		D2D1_FACTORY_OPTIONS opts;
		ZeroMemory(&opts, sizeof(D2D1_FACTORY_OPTIONS));
		opts.debugLevel = D2D1_DEBUG_LEVEL_NONE;
		HRESULT hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &opts, reinterpret_cast<void**>(&m_factory));
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D D2D1CreateFactory Fail\n";
			return FALSE;
		}
		D2D1_SIZE_U size = D2D1::SizeU(cx, cy);
		hRes = m_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWND, size), &m_hwndRenderTarget);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D CreateHwndRenderTarget Fail\n";
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX2D::Initialize(HDC hDC, INT cx, INT cy)
	{
		m_hDC = hDC;
		D2D1_FACTORY_OPTIONS opts;
		ZeroMemory(&opts, sizeof(D2D1_FACTORY_OPTIONS));
		opts.debugLevel = D2D1_DEBUG_LEVEL_NONE;
		HRESULT hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &opts, reinterpret_cast<void**>(&m_factory));
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D D2D1CreateFactory Fail\n";
			return FALSE;
		}
		hRes = m_factory->CreateDCRenderTarget(&D2D1::RenderTargetProperties(), &m_dcRenderTarget);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D CreateDCRenderTarget Fail\n";
			return FALSE;
		}
		return m_dcRenderTarget->BindDC(hDC, TinyRectangle(0, 0, cx, cy));
	}
	BOOL DX2D::BeginDraw()
	{
		if (m_hwndRenderTarget != NULL)
		{
			if (m_hwndRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
				return FALSE;
			m_hwndRenderTarget->BeginDraw();
			m_hwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_hwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			return TRUE;
		}
		if (m_dcRenderTarget != NULL)
		{
			m_dcRenderTarget->BeginDraw();
			m_dcRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_dcRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX2D::EndDraw()
	{
		HRESULT hRes = S_OK;
		if (m_hwndRenderTarget != NULL)
		{
			hRes = m_hwndRenderTarget->EndDraw();
			if (hRes == D2DERR_RECREATE_TARGET)
			{
				hRes = S_OK;
				m_factory.Release();
				m_hwndRenderTarget.Release();
			}
		}
		if (m_dcRenderTarget != NULL)
		{
			hRes = m_dcRenderTarget->EndDraw();
			if (hRes == D2DERR_RECREATE_TARGET)
			{
				hRes = S_OK;
				m_factory.Release();
				m_dcRenderTarget.Release();
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
	ID2D1RenderTarget* DX2D::GetCanvas() const
	{
		if (m_hwndRenderTarget != NULL)
			return static_cast<ID2D1RenderTarget*>(m_hwndRenderTarget);
		if (m_dcRenderTarget != NULL)
			return static_cast<ID2D1RenderTarget*>(m_dcRenderTarget);
		return NULL;
	}
	HWND DX2D::GetHWND() const
	{
		return m_hWND;
	}
	HDC  DX2D::GetDC() const
	{
		return m_hDC;
	}
}

