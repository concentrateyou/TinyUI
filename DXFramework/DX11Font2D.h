#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "DX11Image2D.h"
#include <Richedit.h>

namespace DXFramework
{
	void CHARFORMAT2LOGFONT(const CHARFORMAT& cf, LOGFONT& lf, COLORREF& color);
	void LOGFONT2CHARFORMAT(const LOGFONT& lf, CHARFORMAT& cf, const COLORREF& color);
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const Gdiplus::Font* font);
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const CHARFORMAT& cf);
	/// <summary>
	/// DX11×ÖÌå
	/// </summary>
	class DX11Font2D : public DX11Image2D
	{
		DECLARE_DYNAMIC(DX11Font2D)
	public:
		DX11Font2D();
		virtual ~DX11Font2D();
		BOOL CreateCompatible(DX11& dx11, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor);
		BOOL DrawString(DX11& dx11, const TinyString& str, const RectF& rectF, const StringFormat* format);
		BOOL ClearContext();
	private:
		CHARFORMAT	m_cf;
		COLORREF	m_textColor;
		COLORREF	m_bkColor;
	};
}



