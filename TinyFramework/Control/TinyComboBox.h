#pragma once
#include "TinyControl.h"

namespace TinyFramework
{
	class TinyComboBox : public TinyControl
	{
		DECLARE_DYNAMIC(TinyComboBox)
	public:
		TinyComboBox();
		virtual ~TinyComboBox();
		virtual LPCSTR RetrieveClassName();
		virtual LPCSTR RetrieveTitle();
		virtual DWORD RetrieveStyle();
		virtual DWORD RetrieveExStyle();
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		BOOL SubclassChildren();
	public:
		LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnCommandReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		INT GetCount() const;
		INT GetCurSel() const;
		INT SetCurSel(INT nSelect);
		LCID GetLocale() const;
		LCID SetLocale(LCID nNewLocale);
		INT GetTopIndex() const;
		INT SetTopIndex(INT nIndex);
		INT InitStorage(INT nItems, UINT nBytes);
		void SetHorizontalExtent(UINT nExtent);
		UINT GetHorizontalExtent() const;
		INT SetDroppedWidth(UINT nWidth);
		INT GetDroppedWidth() const;
		BOOL SetMinVisibleItems(INT iMinVisible);
		INT GetMinVisible() const;
		BOOL SetCueBanner(LPCTSTR lpszText);
		TinyString GetCueBanner() const;
		BOOL GetCueBanner(LPWSTR lpszText, INT cchText) const;
		BOOL GetComboBoxInfo(PCOMBOBOXINFO pcbi) const;
		DWORD GetEditSel() const;
		BOOL LimitText(INT nMaxChars);
		BOOL SetEditSel(INT nStartChar, int nEndChar);
		DWORD_PTR GetItemData(INT nIndex) const;
		INT SetItemData(INT nIndex, DWORD_PTR dwItemData);
		void* GetItemDataPtr(INT nIndex) const;
		INT SetItemDataPtr(INT nIndex, void* pData);
		INT GetLBText(INT nIndex, LPTSTR lpszText) const;
		void GetLBText(INT nIndex, TinyString& rString) const;
		INT GetLBTextLen(INT nIndex) const;
		INT SetItemHeight(INT nIndex, UINT cyItemHeight);
		INT GetItemHeight(INT nIndex) const;
		INT FindStringExact(INT nIndexStart, LPCTSTR lpszFind) const;
		INT SetExtendedUI(BOOL bExtended = TRUE);
		BOOL GetExtendedUI() const;
		void GetDroppedControlRect(LPRECT lprect) const;
		BOOL GetDroppedState() const;
		void ShowDropDown(BOOL bShowIt = TRUE);
		INT AddString(LPCTSTR lpszString);
		INT DeleteString(UINT nIndex);
		INT InsertString(INT nIndex, LPCTSTR lpszString);
		void ResetContent();
		INT Dir(UINT attr, LPCTSTR lpszWildCard);
		INT FindString(INT nStartAfter, LPCTSTR lpszString) const;
		INT SelectString(INT nStartAfter, LPCTSTR lpszString);
		void Clear();
		void Copy();
		void Cut();
		void Paste();
	public:
		virtual void OnSelectChange(INT index);
		Event<void(INT)> EVENT_SELECTCHANGE;
	};
}



