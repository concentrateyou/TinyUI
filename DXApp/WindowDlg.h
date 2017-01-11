#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyButton.h"
#include "Control/TinyCheckBox.h"
#include "FindTool.h"
using namespace TinyUI;

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
private:
	FindTool		m_tool;
	TinyCheckBox	m_checkbox;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onCheckClick;
};

