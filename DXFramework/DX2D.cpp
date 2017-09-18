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
		HRESULT hRes = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(&m_factory));
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D D2D1CreateFactory Fail\n";
			return FALSE;
		}
		D2D1_SIZE_U size = D2D1::SizeU(cx, cy);
		hRes = m_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWND, size), &m_renderTarget);
		if (hRes != S_OK)
		{
			LOG(ERROR) << "DX2D CreateHwndRenderTarget Fail\n";
			return FALSE;
		}
		return TRUE;
	}

	BOOL DX2D::BeginDraw()
	{
		if (m_renderTarget != NULL)
		{
			if (m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
				return FALSE;
			m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			m_renderTarget->BeginDraw();
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX2D::EndDraw()
	{
		HRESULT hRes = S_OK;
		if (m_renderTarget != NULL)
		{
			HRESULT hRes = m_renderTarget->EndDraw();
			if (hRes == D2DERR_RECREATE_TARGET)
			{
				hRes = S_OK;
				m_factory.Release();
				m_renderTarget.Release();
			}
		}
		return hRes == S_OK;
	}
	BOOL DX2D::Resize(INT cx, INT cy)
	{
		if (m_renderTarget != NULL)
		{
			D2D1_SIZE_U size;
			size.width = cx;
			size.height = cy;
			return SUCCEEDED(m_renderTarget->Resize(size));
		}
		return FALSE;
	}
	ID2D1HwndRenderTarget* DX2D::GetCanvas() const
	{
		return m_renderTarget;
	}
	HWND DX2D::GetHWND() const
	{
		return m_hWND;
	}
}

