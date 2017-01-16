#pragma once
#include "Common/TinyDialog.h"
#include "Control/TinyButton.h"
#include "Control/TinyTextBox.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyCustomDialog.h"
#include "DX11Font.h"
using namespace TinyUI;
using namespace DXFramework;;

namespace DXApp
{
	class TextDlg : public TinyCustomDialog
	{
		DECLARE_DYNAMIC(TextDlg)
	public:
		TextDlg();
		virtual ~TextDlg();
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		CHARFORMAT		GetFormat() const;
		TinyString		GetText() const;
		COLORREF		GetBkColor() const;
	private:
		void OnFontClick(void*, INT);
		void OnColorClick(void*, INT);
	protected:
		TinyString		m_text;
		CHARFORMAT		m_cf;
		COLORREF		m_bkColor;
		Gdiplus::SizeF	m_sizeF;
		TinyRichTextBox m_txtContext;
		TinyButton		m_btnFont;
		TinyButton		m_btnColor;
		TinyTextBox		m_txtSize;
		HINSTANCE		m_hInstance;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onFontClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onColorClick;



	};
}


