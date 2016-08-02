#include "../stdafx.h"
#include <comdef.h>
#include "TinyRichText.h"

namespace TinyUI
{
	TinyRichText::TinyRichText()
	{

	}
	TinyRichText::~TinyRichText()
	{

	}
	BOOL TinyRichText::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
			return TRUE;
		default:
			if (m_texthost)
			{
				BOOL bHandled = FALSE;
				lResult = m_texthost->TxWindowProc(hWnd, uMsg, wParam, lParam, bHandled);
				return bHandled;
			}
			return TinyControl::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
		}
	}

	LPCSTR TinyRichText::RetrieveClassName()
	{
		return TEXT("WindowlessRichText");
	}
	LPCSTR TinyRichText::RetrieveTitle()
	{
		return TEXT("WindowlessRichText");
	}
	DWORD TinyRichText::RetrieveStyle()
	{
		return (ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL);
	}
	DWORD TinyRichText::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING);
	}
	BOOL TinyRichText::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	LRESULT TinyRichText::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_texthost.Reset(new TinyTextHost(this));
		m_texthost->Initialize();
		return FALSE;
	}
	LRESULT TinyRichText::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}