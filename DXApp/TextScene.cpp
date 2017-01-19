#include "stdafx.h"
#include "TextScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(TextScene, DX11Font);
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
		wstring ws = StringToWString(m_text.STR());
		m_timerID = SetTimer(NULL, NULL, 100, &TextScene::TimerProc);
		Destory();
		if (DX11Font::Create(dx11, ws, cf, bkColor))
		{
			m_displaySize = m_size.cx;
			m_maxSize = m_size.cx;
			return TRUE;
		}
		return FALSE;
	}
	void CALLBACK TextScene::TimerProc(HWND  hwnd, UINT  uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		if (idEvent == m_timerID)
		{
			if (m_displaySize > -(m_maxSize + 200))
			{
				m_displaySize -= 10;
			}
			else
			{
				m_displaySize = m_maxSize + m_displaySize + 200 - 10;
			}
		}
	}

	BOOL TextScene::Allocate(DX11& dx11)
	{
		return TRUE;
	}

	void TextScene::Deallocate(DX11& dx11)
	{
		KillTimer(NULL, m_timerID);
		m_displaySize = m_maxSize;
	}

	BOOL TextScene::Render(DX11& dx11)
	{
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
		format.SetTrimming(Gdiplus::StringTrimmingWord);
		DX11Font::Clear();
		Gdiplus::RectF rectF;
		rectF.X = m_displaySize;
		rectF.Y = 0;
		rectF.Width = m_size.cx;
		rectF.Height = m_size.cy;
		DX11Font::DrawString(dx11, m_text, rectF, &format);
		if (m_displaySize < -200)
		{
			rectF.X = m_maxSize + m_displaySize + 200;
			rectF.Y = 0;
			rectF.Width = m_size.cx;
			rectF.Height = m_size.cy;
			DX11Font::DrawString(dx11, m_text, rectF, &format);
		}
		DX11Image::Render(dx11);
		return TRUE;
	}
}
