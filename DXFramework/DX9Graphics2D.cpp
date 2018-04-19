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
	BOOL DX9Graphics2D::DrawImage(DX9Image2D* ps, FLOAT ratioX, FLOAT ratioY, FLOAT rotate, BOOL bFlipH, BOOL bFlipV)
	{
		if (!ps)
			return FALSE;
		if (ps->IsEmpty())
			return FALSE;
		if (!ps->Translate(m_dx9, ratioX, ratioY, rotate, bFlipH, bFlipV))
			return FALSE;
		if (!ps->Process(m_dx9))
			return FALSE;
		return TRUE;
	}
	BOOL DX9Graphics2D::DrawString(DX9Font2D* ps, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color)
	{
		if (!ps)
			return FALSE;
		return ps->DrawString(m_dx9, pzText, count, pRect, dwFormat, color);
	}
	void DX9Graphics2D::SetRenderView(DX9RenderView* render2D)
	{
		m_dx9.SetRenderView(render2D);
	}
	DX9RenderView*	DX9Graphics2D::GetRenderView() const
	{
		return m_dx9.GetRenderView();
	}
	void DX9Graphics2D::Enter()
	{
		m_dx9.Lock();
	}
	void DX9Graphics2D::Leave()
	{
		m_dx9.Unlock();
	}
}
