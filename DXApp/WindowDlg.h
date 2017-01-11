#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyButton.h"
#include "Control/TinyCheckBox.h"
#include "Control/TinyTextBox.h"
#include "FindWindowTool.h"
using namespace TinyUI;

namespace DXApp
{
	class WindowDlg : public TinyCustomDialog
	{
		DECLARE_DYNAMIC(WindowDlg)
	public:
		WindowDlg();
		virtual ~WindowDlg();
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		HWND GetHWND();
	private:
		void OnCheckClick(void*, INT);
		void OnWindowChange(HWND hWND);
	private:
		HWND			m_lastHWND;
		TinyCheckBox	m_checkbox;
		TinyTextBox		m_txtCaption;
		TinyTextBox		m_txtClass;
		TinyTextBox		m_txtSize;
		FindWindowTool	m_tool;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onCheckClick;
		TinyScopedPtr<Delegate<void(HWND)>>			m_onWindowChange;
	};
}



