#include "stdafx.h"
#include "DX11Font2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Font2D, DX11Image2D);
	DX11Font2D::DX11Font2D()
	{
	}

	DX11Font2D::~DX11Font2D()
	{
	}
	BOOL DX11Font2D::Create(DX11& dx11, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor)
	{
		if (str.empty())
			return FALSE;
		m_bkColor = bkColor;
		m_textColor = cf.crTextColor;
		m_cf = cf;
		HDC hDC = ::GetDC(NULL);
		if (hDC != NULL)
		{
			Gdiplus::SizeF sizeF;
			Gdiplus::RectF rectF = MeasureString(str, cf);
			::ReleaseDC(NULL, hDC);
			rectF.GetSize(&sizeF);
			sizeF.Width += 1;
			sizeF.Height += 1;
			TinyRectangle s = { 0 };
			GetClientRect(dx11.GetHWND(), &s);
			if (sizeF.Width > s.Width())
			{
				sizeF.Width = static_cast<Gdiplus::REAL>(s.Width());
			}
			if (sizeF.Height > s.Height())
			{
				sizeF.Height = static_cast<Gdiplus::REAL>(s.Height());
			}
			return DX11Image2D::Create(dx11, (INT)sizeF.Width, (INT)sizeF.Height);
		}
		return FALSE;
	}
	BOOL DX11Font2D::ClearContext()
	{
		HDC hDC = NULL;
		if (!DX11Texture2D::GetDC(FALSE, hDC))
			return FALSE;
		Graphics g(hDC);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Color color;
		color.SetFromCOLORREF(m_bkColor);
		g.Clear(color);
		g.ReleaseHDC(hDC);
		if (!DX11Texture2D::ReleaseDC())
			return FALSE;
		return TRUE;
	}
	BOOL DX11Font2D::DrawString(DX11& dx11, const TinyString& str, const RectF& rectF, const StringFormat* format)
	{
		if (DX11Texture2D::IsEmpty())
			return FALSE;
		HDC hDC = NULL;
		if (!DX11Texture2D::GetDC(FALSE, hDC))
			return FALSE;
		Graphics g(hDC);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		g.SetClip(RectF(0.0F, 0.0F, static_cast<Gdiplus::REAL>(m_size.x), static_cast<Gdiplus::REAL>(m_size.y)));
		wstring ws = StringToWString(str.STR());
		LOGFONT lf;
		CHARFORMAT2LOGFONT(m_cf, lf, m_textColor);
		Color color;
		color.SetFromCOLORREF(m_textColor);
		SolidBrush textBrush(color);
		Gdiplus::Font font(hDC, &lf);
		g.DrawString(ws.c_str(), -1, &font, rectF, format, &textBrush);
		g.ReleaseHDC(hDC);
		if (!DX11Texture2D::ReleaseDC())
			return FALSE;
		return TRUE;
	}
}