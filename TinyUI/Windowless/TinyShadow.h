#pragma once
#include "../Control/TinyControl.h"

namespace TinyUI
{
	/// <summary>
	/// ÒõÓ°´°¿Ú
	/// </summary>
	class TinyShadow : public TinyControl
	{
		DECLARE_DYNAMIC(TinyShadow)
	public:
		TinyShadow();
		virtual ~TinyShadow();
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		virtual BOOL Create(HWND hParent, HWND hOwner, INT x, INT y, INT cx, INT cy);
	public:
		void SetShadowColor(COLORREF clrInner, COLORREF clrOuter, BYTE alpha);
		void SetShadowDimensions(const RECT& rcSides, UINT uCornerRadius);
	public:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		BOOL			m_bAlphaSupported;
		BYTE			m_cAlpha;
		TinyRectangle	m_rcSides;
		UINT			m_uRadius;
		HWND			m_hOwner;
		COLORREF		m_clrShadowInner;
		COLORREF		m_clrShadowOuter;
	};
}