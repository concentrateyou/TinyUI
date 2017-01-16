#include "stdafx.h"
#include "TextScene.h"

namespace DXApp
{
	TextScene::TextScene()
		:m_bkColor(RGB(255, 255, 255))
	{
	}


	TextScene::~TextScene()
	{
	}

	BOOL TextScene::Initialize(DX11& dx11, const TinyString& text, const CHARFORMAT& cf, const COLORREF& bkColor)
	{
		m_cf = cf;
		m_bkColor = bkColor;
		m_text = std::move(text);
		Destory();
		LOGFONT lf;
		COLORREF color;
		CHARFORMAT2LOGFONT(m_cf, lf, color);
		HDC hDC = GetDC(NULL);
		m_font.Reset(new Gdiplus::Font(hDC, &lf));
		ReleaseDC(NULL, hDC);
		wstring ws = StringToWString(m_text.STR());
		return DX11Font::Create(dx11, ws, cf, bkColor);
	}

	LPCSTR TextScene::GetClassName()
	{
		return TEXT("TextScene");
	}

	DXFramework::ElementType TextScene::GetElementType() const
	{
		return IMAGE;
	}

	BOOL TextScene::BeginScene()
	{
		return TRUE;
	}

	void TextScene::EndScene()
	{
		m_font.Reset(NULL);
	}

	BOOL TextScene::Render(DX11& dx11)
	{
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
		format.SetTrimming(Gdiplus::StringTrimmingWord);
		PointF pos;
		Color color;
		color.SetFromCOLORREF(m_cf.crTextColor);
		SolidBrush brush(color);
		DX11Font::DrawString(dx11, m_text, m_font, pos, &format, &brush);
		DX11Image::Render(dx11);
		return TRUE;
	}
}
