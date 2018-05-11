#include "stdafx.h"
#include "DX11Font2D.h"

namespace DXFramework
{
	void CHARFORMAT2LOGFONT(const CHARFORMAT& cf, LOGFONT& lf, COLORREF& color)
	{
		::ZeroMemory(&lf, sizeof(CHARFORMAT));
		lf.lfCharSet = cf.bCharSet;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		lf.lfWeight = (cf.dwEffects & CFE_BOLD) == CFE_BOLD ? FW_BOLD : 0;
		TinyDC dc(CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL));
		lf.lfHeight = -MulDiv(cf.yHeight / 20, dc.GetDeviceCaps(LOGPIXELSY), 72);
		lf.lfUnderline = ((cf.dwEffects & CFE_UNDERLINE) == CFE_UNDERLINE);
		lf.lfStrikeOut = ((cf.dwEffects & CFE_STRIKEOUT) == CFE_STRIKEOUT);
		lf.lfItalic = ((cf.dwEffects & CFE_ITALIC) == CFE_ITALIC);
		lf.lfWidth = 0;
		_tcscpy_s(lf.lfFaceName, LF_FACESIZE, cf.szFaceName);
		color = cf.crTextColor;
	}
	void LOGFONT2CHARFORMAT(const LOGFONT& lf, CHARFORMAT& cf, const COLORREF& color)
	{
		::ZeroMemory(&cf, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_OFFSET | CFM_PROTECTED | CFM_SIZE | CFM_STRIKEOUT | CFM_UNDERLINE;
		cf.dwEffects = CFE_AUTOCOLOR | (lf.lfWeight >= FW_BOLD ? CFE_BOLD : 0) | (lf.lfItalic ? CFE_ITALIC : 0) | (lf.lfUnderline ? CFE_UNDERLINE : 0) | (lf.lfStrikeOut ? CFE_STRIKEOUT : 0);
		TinyDC dc(CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL));
		cf.yHeight = 20 * LONG(0.5 + fabs(DOUBLE(72 * lf.lfHeight) / dc.GetDeviceCaps(LOGPIXELSY)));
		cf.yOffset = 0;
		cf.crTextColor = color;
		cf.bCharSet = lf.lfCharSet;
		cf.bPitchAndFamily = lf.lfPitchAndFamily;
		_tcscpy_s(cf.szFaceName, LF_FACESIZE, lf.lfFaceName);
	}
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const CHARFORMAT& cf)
	{
		HDC hDC = GetDC(NULL);
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::RectF boundingBox;
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
		format.SetTrimming(Gdiplus::StringTrimmingWord);
		LOGFONT lf;
		COLORREF color;
		CHARFORMAT2LOGFONT(cf, lf, color);
		Gdiplus::Font font(hDC, &lf);
		graphics.MeasureString(str.c_str(), str.size(), &font, PointF(0.0, 0.0), &format, &boundingBox);
		graphics.ReleaseHDC(hDC);
		ReleaseDC(NULL, hDC);
		return boundingBox;
	}
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const Gdiplus::Font* font)
	{
		HDC hDC = GetDC(NULL);
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::RectF boundingBox;
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
		format.SetTrimming(Gdiplus::StringTrimmingWord);
		graphics.MeasureString(str.c_str(), str.size(), font, PointF(0.0, 0.0), &format, &boundingBox);
		graphics.ReleaseHDC(hDC);
		ReleaseDC(NULL, hDC);
		return boundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
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
		g.SetClip(RectF(0.0F, 0.0F, static_cast<Gdiplus::REAL>(m_size.cx), static_cast<Gdiplus::REAL>(m_size.cy)));
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