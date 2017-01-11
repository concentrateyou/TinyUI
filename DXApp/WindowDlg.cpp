#include "stdafx.h"
#include "WindowDlg.h"
#include "Resource.h"

WindowDlg::WindowDlg()
{
}


WindowDlg::~WindowDlg()
{
}

LRESULT WindowDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_tool.SubclassDlgItem(IDC_STATIC_DRAG, m_hWND);
	m_tool.Load();
	m_checkbox.SubclassDlgItem(IDC_CHECK_HIDDEN, m_hWND);
	m_onCheckClick.Reset(new Delegate<void(void*, INT)>(this, &WindowDlg::OnCheckClick));
	m_checkbox.EVENT_CLICK += m_onCheckClick;
	return FALSE;
}
LRESULT WindowDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	switch (LOWORD(wParam))
	{
	case IDOK:
	case IDCANCEL:
		if (EndDialog(LOWORD(wParam)))
		{
			m_checkbox.EVENT_CLICK -= m_onCheckClick;
		}
		break;
	}
	return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
}

void WindowDlg::OnCheckClick(void*, INT)
{
	m_tool.ShowParent(m_checkbox.GetCheck() != BST_CHECKED);
}
HWND WindowDlg::GetHWND()
{
	return m_tool.GetHWND();
}