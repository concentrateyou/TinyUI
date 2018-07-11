#include "stdafx.h"
#include "DX9Graphics2D.h"

namespace DXFramework
{
	DX9Graphics2D::DX9Graphics2D()
	{

	}
	DX9Graphics2D::~DX9Graphics2D()
	{

	}
	BOOL DX9Graphics2D::IsActive() const
	{
		return m_dx9.IsActive();
	}
	DX9& DX9Graphics2D::GetDX9()
	{
		return m_dx9;
	}
	BOOL DX9Graphics2D::Reset()
	{
		return m_dx9.Reset();
	}
	BOOL DX9Graphics2D::Present()
	{
		return m_dx9.Present();
	}
	BOOL DX9Graphics2D::Initialize(HWND hWND, const TinySize& size)
	{
		if (!m_dx9.Initialize(hWND, size.cx, size.cy))
			return FALSE;
		return TRUE;
	}
	BOOL DX9Graphics2D::Resize(const TinySize& size)
	{
		return m_dx9.ResizeView(size.cx, size.cy);
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
	void DX9Graphics2D::SetRenderView(DX9RenderView* render2D)
	{
		m_dx9.SetRenderView(render2D);
	}
	DX9RenderView*	DX9Graphics2D::GetRenderView() const
	{
		return m_dx9.GetRenderView();
	}
}
