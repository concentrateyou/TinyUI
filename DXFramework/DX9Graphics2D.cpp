#include "stdafx.h"
#include "DX9Graphics2D.h"

namespace DXFramework
{
	DX9Graphics2D::DX9Graphics2D(DX9& dx9)
		:m_dx9(dx9)
	{
	}

	DX9Graphics2D::~DX9Graphics2D()
	{
	}
	DX9& DX9Graphics2D::GetDX9()
	{
		return m_dx9;
	}
	TinySize DX9Graphics2D::GetSize() const
	{
		return m_size;
	}
	BOOL DX9Graphics2D::Create()
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
	BOOL DX9Graphics2D::Create(INT cx, INT cy)
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
	void DX9Graphics2D::Destory()
	{
		m_render2D.Release();
	}
	BOOL DX9Graphics2D::Resize()
	{
		Destory();
		return Create();
	}
	BOOL DX9Graphics2D::Resize(INT cx, INT cy)
	{
		Destory();
		return Create(cx, cy);
	}
	BOOL DX9Graphics2D::BeginDraw()
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
		return TRUE;
	}
	BOOL DX9Graphics2D::EndDraw()
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

	BOOL DX9Graphics2D::DrawImage(DX9Image2D* image)
	{
		if (!image || image->IsEmpty())
			return FALSE;
		if (!image->Process(m_dx9))
			return FALSE;
		return TRUE;
	}
	BOOL DX9Graphics2D::DrawString(DX9Font2D* ps, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color)
	{
		if (!ps)
			return FALSE;
		return ps->DrawString(m_dx9, pzText, count, pRect, dwFormat, color);
	}
	BOOL DX9Graphics2D::DrawLine(DX9Line2D* ps, D3DXVECTOR2 *pVertexList, DWORD dwVertexListCount, D3DCOLOR color)
	{
		if (!ps)
			return FALSE;
		return ps->DrawLine(m_dx9, pVertexList, dwVertexListCount, color);
	}
}
