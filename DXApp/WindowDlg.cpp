#include "stdafx.h"
#include "WindowDlg.h"
#include "Resource.h"
#include <string>
using namespace std;

namespace DXApp
{
	IMPLEMENT_DYNAMIC(WindowDlg, TinyCustomDialog);
	WindowDlg::WindowDlg()
		:m_lastHWND(NULL)
	{
	}


	WindowDlg::~WindowDlg()
	{
	}

	LRESULT WindowDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_tool.SubclassDlgItem(IDC_STATIC_DRAG, m_hWND);
		m_tool.LoadResource();
		m_checkbox.SubclassDlgItem(IDC_CHECK_HIDDEN, m_hWND);
		m_txtCaption.SubclassDlgItem(IDC_EDIT_CAPTION, m_hWND);
		m_txtClass.SubclassDlgItem(IDC_EDIT_CLASS, m_hWND);
		m_txtSize.SubclassDlgItem(IDC_EDIT_SIZE, m_hWND);
		m_onCheckClick.Reset(new Delegate<void(void*, INT)>(this, &WindowDlg::OnCheckClick));
		m_onWindowChange.Reset(new Delegate<void(HWND)>(this, &WindowDlg::OnWindowChange));
		m_checkbox.EVENT_CLICK += m_onCheckClick;
		m_tool.EVENT_WINDOWCHANGE += m_onWindowChange;
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
				m_tool.EVENT_WINDOWCHANGE -= m_onWindowChange;
			}
			break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
	}

	void WindowDlg::OnCheckClick(void*, INT)
	{
		m_tool.ShowTool(m_checkbox.GetCheck() != BST_CHECKED);
	}

	void WindowDlg::OnWindowChange(HWND hWND)
	{
		if (hWND != NULL)
		{
			CHAR szClass[MAX_PATH];
			::GetClassName(hWND, szClass, MAX_PATH);
			CHAR szText[MAX_PATH];
			::GetWindowText(hWND, szText, MAX_PATH);
			m_txtCaption.SetText(szText);
			m_txtClass.SetText(szClass);
			TinyRectangle s;
			GetWindowRect(hWND, &s);
			string str = StringPrintf("%d,%d", s.Width(), s.Height());
			m_txtSize.SetText(str.c_str());
			m_lastHWND = hWND;
		}
	}

	HWND WindowDlg::GetHWND()
	{
		return m_lastHWND;
	}
}