#include "stdafx.h"
#include "DX9RenderView.h"

namespace DXFramework
{
	DX9RenderView::DX9RenderView(DX9& dx9)
		:m_dx9(dx9)
	{
	}

	DX9RenderView::~DX9RenderView()
	{
	}
	DX9& DX9RenderView::GetDX9()
	{
		return m_dx9;
	}
	TinySize DX9RenderView::GetSize() const
	{
		return m_size;
	}
	BOOL DX9RenderView::Create()
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx9.GetD3D()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_render2D);
		if (hRes != S_OK)
			return FALSE;
		D3DSURFACE_DESC desc;
		hRes = m_render2D->GetDesc(&desc);
		if (hRes != S_OK)
			return FALSE;
		m_size.cx = desc.Width;
		m_size.cy = desc.Height;
		return TRUE;
	}
	BOOL DX9RenderView::Create(INT cx, INT cy)
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		TinyComPtr<IDirect3DSurface9> render2D;
		HRESULT hRes = m_dx9.GetD3D()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &render2D);
		if (hRes != S_OK)
			return FALSE;
		D3DSURFACE_DESC desc;
		hRes = render2D->GetDesc(&desc);
		if (hRes != S_OK)
			return FALSE;
		m_size.cx = desc.Width;
		m_size.cy = desc.Height;
		hRes = m_dx9.GetD3D()->CreateRenderTarget(desc.Width, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &m_render2D, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX9RenderView::Destory()
	{
		m_render2D.Release();
	}
	BOOL DX9RenderView::Resize()
	{
		Destory();
		return Create();
	}
	BOOL DX9RenderView::Resize(INT cx, INT cy)
	{
		Destory();
		return Create(cx, cy);
	}
	IDirect3DSurface9*	DX9RenderView::GetSurface() const
	{
		return m_render2D;
	}
	BOOL DX9RenderView::BeginDraw()
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx9.GetD3D()->SetRenderTarget(0, m_render2D);
		if (hRes != S_OK)
		{
			TRACE("[BeginDraw] SetRenderTarget:%d\n", hRes);
			LOG(ERROR) << "[BeginDraw] SetRenderTarget::" << hRes;
			return FALSE;
		}
		hRes = m_dx9.GetD3D()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0, 255), 1.0F, 0);
		if (hRes != S_OK)
		{
			TRACE("[BeginDraw] Clear:%d\n", hRes);
			LOG(ERROR) << "[BeginDraw] Clear:" << hRes;
			return FALSE;
		}
		hRes = m_dx9.GetD3D()->BeginScene();
		if (hRes != S_OK)
		{
			TRACE("[BeginDraw] BeginScene:%d\n", hRes);
			LOG(ERROR) << "[BeginDraw] BeginScene::" << hRes;
			return FALSE;
		}
		hRes = m_dx9.GetD3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		if (hRes != S_OK)
		{
			TRACE("[BeginDraw] SetFVF:%d\n", hRes);
			LOG(ERROR) << "[BeginDraw] SetFVF::" << hRes;
			return FALSE;
		}
		hRes = m_dx9.GetD3D()->SetRenderState(D3DRS_LIGHTING, FALSE);
		if (hRes != S_OK)
		{
			TRACE("[BeginDraw] SetRenderState:%d\n", hRes);
			LOG(ERROR) << "[BeginDraw] SetRenderState::" << hRes;
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX9RenderView::EndDraw()
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx9.GetD3D()->EndScene();
		if (hRes != S_OK)
		{
			TRACE("[EndDraw] EndScene:%d\n", hRes);
			LOG(ERROR) << "[EndDraw] EndScene::" << hRes;
			return FALSE;
		}
		return TRUE;
	}
}
