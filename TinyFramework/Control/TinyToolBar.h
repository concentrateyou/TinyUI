#pragma once
#include "TinyControl.h"

namespace TinyFramework
{
#define STRING_REFRESH_OFFSET 1000000

	class TinyToolBar : public TinyControl
	{
		DECLARE_DYNAMIC(TinyToolBar)
	public:
		TinyToolBar();
		virtual ~TinyToolBar();
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		virtual BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	public:
		void SetSizes(SIZE sizeButton, SIZE sizeImage);
		BOOL SetButtons(const UINT* lpIDArray, INT nIDCount);
		INT CommandToIndex(UINT nIDFind) const;
		UINT GetItemID(INT nIndex) const;
		virtual void GetItemRect(INT nIndex, LPRECT lpRect) const;
		UINT GetButtonStyle(INT nIndex) const;
		void SetButtonStyle(INT nIndex, UINT nStyle);
		void GetButtonInfo(INT nIndex, UINT& nID, UINT& nStyle, INT& iImage) const;
		void SetButtonInfo(INT nIndex, UINT nID, UINT nStyle, INT iImage);
		BOOL SetButtonText(INT nIndex, LPCTSTR lpszText);
		TinyString GetButtonText(INT nIndex) const;
		void GetButtonText(INT nIndex, TinyString& str) const;
	protected:
		void GetButton(INT nIndex, TBBUTTON* pButton) const;
		void SetButton(INT nIndex, TBBUTTON* pButton);
	public:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		TinySize m_sizeImage;
		TinySize m_sizeButton;

	};
}



