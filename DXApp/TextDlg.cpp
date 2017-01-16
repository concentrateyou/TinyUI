#include "stdafx.h"
#include "TextDlg.h"
#include "Resource.h"
#include "Utility.h"
#include "Control/TinyCommonDialog.h"


namespace DXApp
{
	TextDlg::TextDlg()
	{
		m_hInstance = LoadLibrary("Riched20.dll");
		m_onFontClick.Reset(new Delegate<void(void*, INT)>(this, &TextDlg::OnFontClick));
		m_onColorClick.Reset(new Delegate<void(void*, INT)>(this, &TextDlg::OnColorClick));
		m_btnFont.EVENT_CLICK += m_onFontClick;
		m_btnColor.EVENT_CLICK += m_onColorClick;
	}

	TextDlg::~TextDlg()
	{
		m_btnFont.EVENT_CLICK -= m_onFontClick;
		m_btnColor.EVENT_CLICK -= m_onColorClick;
		if (m_hInstance != NULL)
		{
			FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
	}

	LRESULT TextDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_txtContext.SubclassDlgItem(IDC_RICHEDIT2_TEXT, m_hWND);
		m_txtContext.ModifyStyle(0, ES_MULTILINE);
		m_btnFont.SubclassDlgItem(IDC_BTN_FONT, m_hWND);
		m_btnColor.SubclassDlgItem(IDC_BTN_COLOR, m_hWND);
		m_txtSize.SubclassDlgItem(IDC_EDIT_SIZE, m_hWND);
		return FALSE;
	}

	LRESULT TextDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
		{
			m_txtContext.GetText(m_text);
			if (EndDialog(LOWORD(wParam)))
			{

			}
		}
		break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
	}
	void TextDlg::OnFontClick(void*, INT)
	{
		TinyFontDialog dlg;
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			dlg.GetCharFormat(m_cf);
			m_txtContext.SetDefaultCharFormat(m_cf);
			m_txtContext.Invalidate();
			TinyString str;
			m_txtContext.GetText(str);
			wstring ws = StringToWString(str.STR());
			Gdiplus::RectF rectF = MeasureString(ws, m_cf);
			rectF.GetSize(&m_sizeF);
			m_txtSize.SetText(StringPrintf("%f,%f", m_sizeF.Width, m_sizeF.Height).c_str());
		}
	}
	void TextDlg::OnColorClick(void*, INT)
	{
		TinyColorDialog dlg;
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			m_cf.cbSize = sizeof(CHARFORMAT);
			m_txtContext.GetDefaultCharFormat(m_cf);
			m_bkColor = dlg.GetColor();
			m_txtContext.SetBackgroundColor(FALSE, m_bkColor);
			m_txtContext.SetDefaultCharFormat(m_cf);
			m_txtContext.Invalidate();
		}
	}

	CHARFORMAT	TextDlg::GetFormat() const
	{
		return m_cf;
	}
	TinyString	TextDlg::GetText() const
	{
		return m_text;
	}
	COLORREF TextDlg::GetBkColor() const
	{
		return m_bkColor;
	}
}
