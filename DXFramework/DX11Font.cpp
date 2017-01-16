#include "stdafx.h"
#include "DX11Font.h"

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
	Gdiplus::RectF WINAPI MeasureString(HDC hDC, const wstring& str, const CHARFORMAT& cf)
	{
		Gdiplus::Graphics graphics(hDC);
		LOGFONT lf;
		COLORREF color;
		CHARFORMAT2LOGFONT(cf, lf, color);
		Gdiplus::Font font(hDC, &lf);
		Gdiplus::RectF boundingBox;
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
		format.SetTrimming(Gdiplus::StringTrimmingWord);
		graphics.MeasureString(str.c_str(), str.size(), &font, PointF(0.0, 0.0), &format, &boundingBox);
		return boundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	DX11Font::DX11Font()
	{
	}

	DX11Font::~DX11Font()
	{
	}
	BOOL DX11Font::Create(DX11& dx11, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor)
	{
		m_bkColor = bkColor;
		if (str.empty())
			return FALSE;
		HDC hDC = GetDC(NULL);
		if (hDC != NULL)
		{
			Gdiplus::RectF rectF = MeasureString(hDC, str, cf);
			ReleaseDC(NULL, hDC);
			Gdiplus::SizeF sizeF;
			rectF.GetSize(&sizeF);
			return DX11Image::Create(dx11, TinySize((INT)(sizeF.Width + 1), (INT)(sizeF.Height)), NULL);
		}
		return FALSE;
	}
	BOOL DX11Font::DrawString(DX11& dx11, const TinyString& str, const Gdiplus::Font* font, const PointF& pos, const StringFormat* format, const Brush* brush)
	{
		ASSERT(m_texture.IsValid());
		HDC hDC = NULL;
		if (!m_texture.GetDC(hDC))
			return FALSE;
		Graphics g(hDC);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		wstring ws = StringToWString(str.STR());
		g.DrawString(ws.c_str(), -1, font, pos, format, brush);
		g.ReleaseHDC(hDC);
		if (!m_texture.ReleaseDC())
			return FALSE;
		return TRUE;
	}
}