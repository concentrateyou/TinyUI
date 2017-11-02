#pragma once
#include "TinyControl.h"

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
		BOOL Create(HWND hParent, HWND hOwner);
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		void SetShadowColor(COLORREF clrInner, COLORREF clrOuter, BYTE alpha);
		void SetShadowDimensions(const RECT& rcSides, UINT uCornerRadius);
		void TrackWindow();
	public:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		BYTE			m_cAlpha;
		TinyRectangle	m_rcSides;
		UINT			m_uRadius;
		HWND			m_hOwner;
		COLORREF		m_clrShadowInner;
		COLORREF		m_clrShadowOuter;
	};
}