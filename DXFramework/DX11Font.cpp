#include "stdafx.h"
#include "DX11Font.h"

namespace DXFramework
{
	DX11Font::DX11Font()
	{
	}

	DX11Font::~DX11Font()
	{
	}
	BOOL DX11Font::Create(const DX11& dx11, HFONT hFont, const wstring& str)
	{
		if (str.size())
			return FALSE;
		HDC hDC = CreateCompatibleDC(NULL);
		if (hDC != NULL)
		{
			Gdiplus::RectF boundingBox;
			Gdiplus::Font font(hDC, hFont);
			Gdiplus::Graphics graphics(hDC);
			Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
			graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			graphics.MeasureString(str.c_str(), str.size(), &font, PointF(0.0, 0.0), &format, &boundingBox);
			DeleteDC(hDC);
			//return DX11Image::Create(dx11, hFont, str);
		}
		return FALSE;
	}
}