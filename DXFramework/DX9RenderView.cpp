#include "stdafx.h"
#include "DX9RenderView.h"

namespace DXFramework
{
	DX9RenderView::DX9RenderView(DX9& dx9)
		:m_dx9(dx9),
		m_handle(NULL)
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
	BOOL DX9RenderView::BeginDraw()
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx9.GetD3D()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(255, 0, 0, 0), 1.0F, 0);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx9.GetD3D()->BeginScene();
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx9.GetD3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx9.GetD3D()->SetRenderState(D3DRS_LIGHTING, FALSE);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL DX9RenderView::EndDraw()
	{
		if (m_dx9.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx9.GetD3D()->EndScene();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
}
