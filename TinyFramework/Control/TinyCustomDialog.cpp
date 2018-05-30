#include "../stdafx.h"
#include "TinyCustomDialog.h"

namespace TinyFramework
{
	IMPLEMENT_DYNAMIC(TinyCustomDialog, TinyDialog);
	TinyCustomDialog::TinyCustomDialog()
	{

	}
	TinyCustomDialog::~TinyCustomDialog()
	{

	}
	LRESULT TinyCustomDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyCustomDialog::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (lParam != NULL)
		{
			return ::SendMessage((HWND)lParam, (WM_COMMANDREFLECT), wParam, lParam);
		}
		return FALSE;
	}
	LRESULT TinyCustomDialog::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyCustomDialog::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}
