#include "../stdafx.h"
#include "TinyComboBox.h"

namespace TinyUI
{
	TinyComboBox::TinyComboBox()
	{
	};
	TinyComboBox::~TinyComboBox()
	{

	};
	BOOL TinyComboBox::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		INITCOMMONCONTROLSEX initCtrls = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES };
		if (InitCommonControlsEx(&initCtrls))
		{
			return TinyControl::Create(hParent, x, y, cx, cy);
		}
		return FALSE;
	};
	DWORD TinyComboBox::RetrieveStyle()
	{
		return (WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}
	DWORD TinyComboBox::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING);
	}
	LPCSTR TinyComboBox::RetrieveClassName()
	{
		return WC_COMBOBOX;
	};
	LPCSTR TinyComboBox::RetrieveTitle()
	{
		return _T("TinyComboBox");
	};

	INT TinyComboBox::GetCount() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETCOUNT, 0, 0);
	}
	INT TinyComboBox::GetCurSel() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETCURSEL, 0, 0);
	}
	INT TinyComboBox::SetCurSel(INT nSelect)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETCURSEL, nSelect, 0);
	}
	DWORD TinyComboBox::GetEditSel() const
	{
		ASSERT(::IsWindow(m_hWND));
		return DWORD(::SendMessage(m_hWND, CB_GETEDITSEL, 0, 0));
	}
	BOOL TinyComboBox::LimitText(INT nMaxChars)
	{
		ASSERT(::IsWindow(m_hWND));
		return (BOOL)::SendMessage(m_hWND, CB_LIMITTEXT, nMaxChars, 0);
	}
	BOOL TinyComboBox::SetEditSel(INT nStartChar, INT nEndChar)
	{
		ASSERT(::IsWindow(m_hWND));
		return (BOOL)::SendMessage(m_hWND, CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
	}
	DWORD_PTR TinyComboBox::GetItemData(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SendMessage(m_hWND, CB_GETITEMDATA, nIndex, 0);
	}
	int TinyComboBox::SetItemData(INT nIndex, DWORD_PTR dwItemData)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
	}
	void* TinyComboBox::GetItemDataPtr(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (LPVOID)GetItemData(nIndex);
	}
	int TinyComboBox::SetItemDataPtr(INT nIndex, void* pData)
	{
		ASSERT(::IsWindow(m_hWND));
		return SetItemData(nIndex, (DWORD_PTR)(LPVOID)pData);
	}
	INT TinyComboBox::GetLBText(INT nIndex, LPTSTR lpszText) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
	}
	int TinyComboBox::GetLBTextLen(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETLBTEXTLEN, nIndex, 0);
	}
	void TinyComboBox::ShowDropDown(BOOL bShowIt)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, CB_SHOWDROPDOWN, bShowIt, 0);
	}
	INT TinyComboBox::AddString(LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_ADDSTRING, 0, (LPARAM)lpszString);
	}
	int TinyComboBox::DeleteString(UINT nIndex)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_DELETESTRING, nIndex, 0);
	}
	int TinyComboBox::InsertString(INT nIndex, LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
	}
	void TinyComboBox::ResetContent()
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, CB_RESETCONTENT, 0, 0);
	}
	INT TinyComboBox::Dir(UINT attr, LPCTSTR lpszWildCard)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_DIR, attr, (LPARAM)lpszWildCard);
	}
	INT TinyComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_FINDSTRING, nStartAfter, (LPARAM)lpszString);
	}
	INT TinyComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SELECTSTRING, nStartAfter, (LPARAM)lpszString);
	}
	void TinyComboBox::Clear()
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_CLEAR, 0, 0);
	}
	void TinyComboBox::Copy()
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_COPY, 0, 0);
	}
	void TinyComboBox::Cut()
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_CUT, 0, 0);
	}
	void TinyComboBox::Paste()
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_PASTE, 0, 0);
	}

	INT TinyComboBox::SetItemHeight(INT nIndex, UINT cyItemHeight)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
	}
	INT TinyComboBox::GetItemHeight(INT nIndex) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETITEMHEIGHT, nIndex, 0L);
	}
	INT TinyComboBox::FindStringExact(INT nIndexStart, LPCTSTR lpszFind) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
	}
	INT TinyComboBox::SetExtendedUI(BOOL bExtended)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETEXTENDEDUI, bExtended, 0L);
	}
	BOOL TinyComboBox::GetExtendedUI() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (BOOL)::SendMessage(m_hWND, CB_GETEXTENDEDUI, 0, 0L);
	}
	void TinyComboBox::GetDroppedControlRect(LPRECT lprect) const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)lprect);
	}
	BOOL TinyComboBox::GetDroppedState() const
	{
		ASSERT(::IsWindow(m_hWND)); return (BOOL)::SendMessage(m_hWND, CB_GETDROPPEDSTATE, 0, 0L);
	}
	LCID TinyComboBox::GetLocale() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (LCID)::SendMessage(m_hWND, CB_GETLOCALE, 0, 0);
	}
	LCID TinyComboBox::SetLocale(LCID nNewLocale)
	{
		ASSERT(::IsWindow(m_hWND));
		return (LCID)::SendMessage(m_hWND, CB_SETLOCALE, (WPARAM)nNewLocale, 0);
	}
	INT TinyComboBox::GetTopIndex() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETTOPINDEX, 0, 0);
	}
	INT TinyComboBox::SetTopIndex(INT nIndex)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETTOPINDEX, nIndex, 0);
	}
	INT TinyComboBox::InitStorage(INT nItems, UINT nBytes)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_INITSTORAGE, (WPARAM)nItems, nBytes);
	}
	void TinyComboBox::SetHorizontalExtent(UINT nExtent)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, CB_SETHORIZONTALEXTENT, nExtent, 0);
	}
	UINT TinyComboBox::GetHorizontalExtent() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (UINT)::SendMessage(m_hWND, CB_GETHORIZONTALEXTENT, 0, 0);
	}
	INT TinyComboBox::SetDroppedWidth(UINT nWidth)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_SETDROPPEDWIDTH, nWidth, 0);
	}
	INT TinyComboBox::GetDroppedWidth() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, CB_GETDROPPEDWIDTH, 0, 0);
	}
	void TinyComboBox::GetLBText(INT nIndex, TinyString& rString) const
	{
		ASSERT(::IsWindow(m_hWND));
		INT size = GetLBTextLen(nIndex);
		TinyString str(size + 1);
		GetLBText(nIndex, str.STR());
		rString = std::move(str);
	}

	LRESULT TinyComboBox::OnCommandReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			OnSelectChange(GetCurSel());
		}
		return FALSE;
	}

	void TinyComboBox::OnSelectChange(INT index)
	{
		EVENT_SelectChange(index);
	}
}

