#include "../stdafx.h"
#include "TinyShadow.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyShadow, TinyControl);
	TinyShadow::TinyShadow()
		:m_cAlpha(64), m_rcSides(2, 2, 2, 5), m_uRadius(4), m_hOwner(NULL),
		m_clrShadowInner(0x000000), m_clrShadowOuter(0xA0A0A0),
		m_bAlphaSupported(FALSE)
	{

	}
	TinyShadow::~TinyShadow()
	{

	}
	LPCSTR TinyShadow::RetrieveClassName()
	{
		return TEXT("TinyShadow");
	}
	LPCSTR TinyShadow::RetrieveTitle()
	{
		return TEXT("TinyShadow");
	}
	DWORD TinyShadow::RetrieveStyle()
	{
		return WS_POPUP | WS_VISIBLE;
	}
	DWORD TinyShadow::RetrieveExStyle()
	{
		return WS_EX_TRANSPARENT | WS_EX_LAYERED;
	}
	void TinyShadow::SetShadowColor(COLORREF clrInner, COLORREF clrOuter, BYTE alpha)
	{
		m_clrShadowInner = clrInner;
		m_clrShadowOuter = clrOuter;
		m_cAlpha = alpha;
		Invalidate();
	}
	void TinyShadow::SetShadowDimensions(const RECT& rcSides, UINT uCornerRadius)
	{
		m_rcSides.CopyRect(&rcSides);
		m_uRadius = uCornerRadius;
		Invalidate();
	}
	BOOL TinyShadow::Create(HWND hParent, HWND hOwner, INT x, INT y, INT cx, INT cy)
	{
		TinyRectangle s;
		::GetWindowRect(hOwner, &s);
		s.InflateRect(&m_rcSides);
		m_hOwner = hOwner;
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	LRESULT TinyShadow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return FALSE;
	}

}