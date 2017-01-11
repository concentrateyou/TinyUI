#include "../stdafx.h"
#include "TinyCustomDialog.h"

namespace TinyUI
{
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
		else
		{
			bHandled = TRUE;
			//²Ëµ¥ClickÊÂ¼þ
			/*INT wID = LOWORD(wParam);
			SysMenu* menuPtr = (SysMenu*)__Module.GetMapMENUID().FromKey(wID);
			if (menuPtr != NULL)
			{
			menuPtr->OnClick(menuPtr, wID);
			}*/
			return TRUE;
		}
		return FALSE;
	}
}
