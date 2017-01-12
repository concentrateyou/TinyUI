#include "stdafx.h"
#include "TextDlg.h"
#include "Control/TinyCommonDialog.h"
#include "Resource.h"

namespace DXApp
{
	TextDlg::TextDlg()
	{
		m_hInstance = LoadLibrary("Riched20.dll");
	}

	TextDlg::~TextDlg()
	{
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
		m_btnFont.SubclassDlgItem(IDC_BTN_FONT, m_hWND);
		m_btnColor.SubclassDlgItem(IDC_BTN_COLOR, m_hWND);
		m_txtSize.SubclassDlgItem(IDC_RICHEDIT2_TEXT, m_hWND);

		m_onFontClick.Reset(new Delegate<void(void*, INT)>(this, &TextDlg::OnFontClick));
		m_onColorClick.Reset(new Delegate<void(void*, INT)>(this, &TextDlg::OnColorClick));
		m_btnFont.EVENT_CLICK += m_onFontClick;
		m_btnColor.EVENT_CLICK += m_onColorClick;

		return FALSE;
	}

	LRESULT TextDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			if (EndDialog(LOWORD(wParam)))
			{
				m_btnFont.EVENT_CLICK -= m_onFontClick;
				m_btnColor.EVENT_CLICK -= m_onColorClick;
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
			/*CHARFORMAT cf = { 0 };
			cf.cbSize = sizeof(CHARFORMAT);
			m_txtContext.GetDefaultCharFormat(&cf);
			m_txtContext.SetDefaultCharFormat(&cf);
			m_txtContext.Invalidate();*/
		}
	}
	void TextDlg::OnColorClick(void*, INT)
	{
		TinyColorDialog dlg;
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			/*CHARFORMAT cf = { 0 };
			cf.cbSize = sizeof(CHARFORMAT);
			m_txtContext.GetDefaultCharFormat(&cf);
			m_txtContext.SetDefaultCharFormat(&cf);
			m_txtContext.Invalidate();*/
		}
	}
}
