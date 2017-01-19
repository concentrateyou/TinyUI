#include "../stdafx.h"
#include "TinyToolBar.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyToolBar, TinyControl);
	TinyToolBar::TinyToolBar()
	{

	}
	TinyToolBar::~TinyToolBar()
	{

	}
	LPCSTR TinyToolBar::RetrieveClassName()
	{
		return TOOLBARCLASSNAME;
	}
	LPCSTR TinyToolBar::RetrieveTitle()
	{
		return TEXT("TinyToolBar");
	}
	DWORD TinyToolBar::RetrieveStyle()
	{
		return (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | TBSTYLE_TRANSPARENT);
	}
	DWORD TinyToolBar::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE);
	}
	BOOL TinyToolBar::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		INITCOMMONCONTROLSEX initCtrls = { sizeof(INITCOMMONCONTROLSEX),ICC_BAR_CLASSES | ICC_COOL_CLASSES };
		if (InitCommonControlsEx(&initCtrls))
		{
			return TinyControl::Create(hParent, x, y, cx, cy);
		}
		return FALSE;
	}
	void TinyToolBar::SetSizes(SIZE sizeButton, SIZE sizeImage)
	{
		if (::IsWindow(m_hWND))
		{
			::SendMessage(m_hWND, TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
			::SendMessage(m_hWND, TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
			Invalidate();
		}
		else
		{
			m_sizeButton = sizeButton;
			m_sizeImage = sizeImage;
		}
	}
	BOOL TinyToolBar::SetButtons(const UINT* lpIDArray, INT nIDCount)
	{
		INT count = (INT)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
		while (count--)
		{
			ASSERT(DefWindowProc(TB_DELETEBUTTON, 0, 0));
		}
		TBBUTTON button;
		memset(&button, 0, sizeof(TBBUTTON));
		button.iString = -1;
		if (lpIDArray != NULL)
		{
			INT iImage = 0;
			for (INT i = 0; i < nIDCount; i++)
			{
				button.fsState = TBSTATE_ENABLED;
				if ((button.idCommand = *lpIDArray++) == 0)
				{
					button.fsStyle = TBSTYLE_SEP;
					if ((GetStyle() & TBSTYLE_FLAT))
						button.iBitmap = 6;
					else
						button.iBitmap = 8;
				}
				else
				{
					button.fsStyle = TBSTYLE_BUTTON;
					button.iBitmap = iImage++;
				}
				if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
					return FALSE;
			}
		}
		else
		{
			button.fsState = TBSTATE_ENABLED;
			for (int i = 0; i < nIDCount; i++)
			{
				ASSERT(button.fsStyle == TBSTYLE_BUTTON);
				if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
					return FALSE;
			}
		}
		return TRUE;
	}
	LRESULT TinyToolBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		SendMessage(m_hWND, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(m_hWND, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_HIDECLIPPEDBUTTONS | TBSTYLE_EX_DOUBLEBUFFER);
		SendMessage(m_hWND, TB_SETIMAGELIST, 0, 0);
		SendMessage(m_hWND, WM_GETFONT, 0, 0);
		return FALSE;
	}
	INT TinyToolBar::CommandToIndex(UINT nIDFind) const
	{
		ASSERT(::IsWindow(m_hWND));
		TinyToolBar* ps = (TinyToolBar*)this;
		return (INT)ps->DefWindowProc(TB_COMMANDTOINDEX, nIDFind, 0);
	}
	UINT TinyToolBar::GetItemID(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		TBBUTTON button;
		GetButton(nIndex, &button);
		return button.idCommand;
	}
	void TinyToolBar::GetItemRect(INT nIndex, LPRECT lpRect) const
	{
		ASSERT(::IsWindow(m_hWND));
		TinyToolBar* ps = (TinyToolBar*)this;
		if (!ps->DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)lpRect))
		{
			SetRectEmpty(lpRect);
		}
	}
	UINT TinyToolBar::GetButtonStyle(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		TBBUTTON button;
		GetButton(nIndex, &button);
		return MAKELONG(button.fsStyle, button.fsState);
	}
	void TinyToolBar::SetButtonStyle(INT nIndex, UINT nStyle)
	{
		ASSERT(::IsWindow(m_hWND));
		TBBUTTON button;
		GetButton(nIndex, &button);
		if (button.fsStyle != (BYTE)LOWORD(nStyle) || button.fsState != (BYTE)HIWORD(nStyle))
		{
			button.fsStyle = (BYTE)LOWORD(nStyle);
			button.fsState = (BYTE)HIWORD(nStyle);
			SetButton(nIndex, &button);
		}
	}
	void TinyToolBar::GetButtonInfo(INT nIndex, UINT& nID, UINT& nStyle, INT& iImage) const
	{
		ASSERT(::IsWindow(m_hWND));
		TBBUTTON button;
		GetButton(nIndex, &button);
		nID = button.idCommand;
		nStyle = MAKELONG(button.fsStyle, button.fsState);
		iImage = button.iBitmap;
	}
	void TinyToolBar::SetButtonInfo(INT nIndex, UINT nID, UINT nStyle, INT iImage)
	{
		ASSERT(::IsWindow(m_hWND));
		TBBUTTON button;
		GetButton(nIndex, &button);
		TBBUTTON save;
		memcpy_s(&save, sizeof(TBBUTTON), &button, sizeof(TBBUTTON));
		button.idCommand = nID;
		button.iBitmap = iImage;
		button.fsStyle = (BYTE)LOWORD(nStyle);
		button.fsState = (BYTE)HIWORD(nStyle);
		if (memcmp(&save, &button, sizeof(save)) != 0)
		{
			SetButton(nIndex, &button);
		}
	}
	BOOL TinyToolBar::SetButtonText(INT nIndex, LPCTSTR lpszText)
	{
		TinyString str(lpszText);
		INT_PTR nString = (INT_PTR)DefWindowProc(TB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str.STR());
		if (nString == -1)
			return FALSE;
		TBBUTTON button;
		GetButton(nIndex, &button);
		button.iString = nString - STRING_REFRESH_OFFSET;
		SetButton(nIndex, &button);
		return TRUE;
	}
	TinyString TinyToolBar::GetButtonText(INT nIndex) const
	{
		TinyString str;
		GetButtonText(nIndex, str);
		return str;
	}
	void TinyToolBar::GetButtonText(INT nIndex, TinyString& str) const
	{
		TinyToolBar* ps = (TinyToolBar*)this;
		ASSERT(ps->DefWindowProc(TB_GETBUTTONTEXT, nIndex, (LPARAM)(LPCTSTR)str.STR()));
	}

	void TinyToolBar::GetButton(INT nIndex, TBBUTTON* pButton) const
	{
		TinyToolBar* ps = (TinyToolBar*)this;
		ASSERT(ps->DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
		pButton->fsState ^= TBSTATE_ENABLED;
	}
	void TinyToolBar::SetButton(INT nIndex, TBBUTTON* pButton)
	{
		TBBUTTON button;
		ASSERT(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)&button));
		button.bReserved[0] = 0;
		button.bReserved[1] = 0;
		pButton->fsState ^= TBSTATE_ENABLED;
		pButton->bReserved[0] = 0;
		pButton->bReserved[1] = 0;
		if (memcmp(pButton, &button, sizeof(TBBUTTON)) != 0)
		{
			DWORD dwStyle = GetStyle();
			ModifyStyle(WS_VISIBLE, 0);
			ASSERT(DefWindowProc(TB_DELETEBUTTON, nIndex, 0));
			if (pButton->iString < -1)
			{
				INT iTextRows;
				iTextRows = (int)::SendMessage(m_hWND, TB_GETTEXTROWS, 0, 0);
				::SendMessage(m_hWND, WM_SETREDRAW, FALSE, 0);
				::SendMessage(m_hWND, TB_SETMAXTEXTROWS, iTextRows + 1, 0);
				::SendMessage(m_hWND, TB_SETMAXTEXTROWS, iTextRows, 0);
				::SendMessage(m_hWND, WM_SETREDRAW, TRUE, 0);
				pButton->iString += STRING_REFRESH_OFFSET;
			}
			ASSERT(DefWindowProc(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
			ModifyStyle(0, dwStyle & WS_VISIBLE);
			if (((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) || ((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
			{
				Invalidate();
			}
			else
			{
				TinyRectangle rect;
				if (DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
				{
					InvalidateRect(m_hWND, &rect, TRUE);
				}
			}
		}

	}
}

