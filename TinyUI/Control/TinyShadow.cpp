#include "../stdafx.h"
#include "TinyShadow.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyShadow, TinyControl);

	TinyShadow::TinyShadow()
		:m_cAlpha(64), m_rcSides(2, 2, 2, 5), m_uRadius(4), m_hOwner(NULL),
		m_clrShadowInner(0x000000), m_clrShadowOuter(0xA0A0A0)
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
		return WS_POPUPWINDOW | WS_VISIBLE;
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
	BOOL TinyShadow::Create(HWND hParent, HWND hOwner)
	{
		TinyRectangle s;
		::GetWindowRect(hOwner, &s);
		s.InflateRect(&m_rcSides);
		m_hOwner = hOwner;
		return TinyControl::Create(hParent, s.left, s.top, TO_CX(s), TO_CY(s), FALSE);
	}

	LRESULT TinyShadow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps = { 0 };
		HDC hDC = BeginPaint(m_hWND, &ps);
		TinyRectangle rcShadow;
		GetClientRect(&rcShadow);
		COLORREF gray1 = m_clrShadowOuter, gray2 = m_clrShadowInner;
		COLORREF clTransparent = RGB(255, 255, 255);
		TinyMemDC memDC(hDC, TO_CX(rcShadow), TO_CY(rcShadow));
		memDC.FillSolidRect(rcShadow, clTransparent);
		INT cShadowSize = max(m_rcSides.left, max(m_rcSides.top, max(m_rcSides.right, m_rcSides.bottom)));
		TinyPoint ptRound(m_uRadius * 2, m_uRadius * 2);
		TinyBrush brWhite;
		brWhite.CreateBrush(clTransparent);
		HBRUSH hOldBrush = memDC.SelectObject(&brWhite);
		HPEN hOldPen = (HPEN)::GetCurrentObject(memDC.Handle(), OBJ_PEN);
		for (INT i = 0; i < cShadowSize; ++i)
		{
			BYTE r = (BYTE)(GetRValue(gray1) + (GetRValue(gray2) - GetRValue(gray1)) * i / cShadowSize);
			BYTE g = (BYTE)(GetGValue(gray1) + (GetGValue(gray2) - GetGValue(gray1)) * i / cShadowSize);
			BYTE b = (BYTE)(GetBValue(gray1) + (GetBValue(gray2) - GetBValue(gray1)) * i / cShadowSize);
			TinyPen pen;
			pen.CreatePen(PS_SOLID, 2, RGB(r, g, b));
			memDC.SelectObject(&pen);
			if (rcShadow.IsRectEmpty())
			{
				break;
			}
			if (ptRound.x * ptRound.y > 0)
			{
				memDC.RoundRect(&rcShadow, ptRound);
			}
			else
			{
				memDC.Rectangle(&rcShadow);
			}
			rcShadow.DeflateRect(1, 1, 1, 1);
		}
		memDC.SelectObject(hOldPen);
		memDC.SelectObject(hOldBrush);
		if (m_hOwner != NULL)
		{
			TinyRectangle rcHost;
			::GetWindowRect(m_hOwner, &rcHost);
			TinyRgn rgnHost;
			rgnHost.CreateRgn(0, 0, 0, 0);
			INT rgnType = GetWindowRgn(m_hOwner, rgnHost);
			if (rgnType != RGN_ERROR)
			{
				rgnHost.OffsetRgn(-rcHost.left + m_rcSides.left, -rcHost.top + m_rcSides.top);
				memDC.SelectClipRgn(&rgnHost);
				TinyRectangle rcClip;
				memDC.GetClipBox(&rcClip);
				memDC.FillSolidRect(rcClip, clTransparent);
			}
			else
			{
				rcHost.OffsetRect(-rcHost.left + m_rcSides.left, -rcHost.top + m_rcSides.top);
				memDC.FillSolidRect(rcHost, clTransparent);
			}
		}
		memDC.Render(rcShadow, rcShadow, FALSE);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}

	LRESULT TinyShadow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetLayeredWindowAttributes(m_hWND, RGB(255, 255, 255), m_cAlpha, LWA_ALPHA | LWA_COLORKEY);
		return FALSE;
	}
	void TinyShadow::TrackWindow()
	{
		TinyRectangle rectangle;
		::GetWindowRect(m_hOwner, &rectangle);
		rectangle.InflateRect(&m_rcSides);
		::SetWindowPos(m_hWND, m_hOwner, rectangle.left, rectangle.top, rectangle.Width(), rectangle.Height(), SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_SHOWWINDOW);
	}
}