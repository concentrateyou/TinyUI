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
		return (WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
	}
	DWORD TinyRichText::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING);
	}
	BOOL TinyRichText::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}

	LRESULT TinyRichText::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}

	LRESULT TinyRichText::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}

	LRESULT TinyRichText::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		::SetFocus(m_hWND);
		return lRes;
	}

	LRESULT TinyRichText::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		LRESULT lRes = 0;
		POINT pos = { 0 };
		GetCursorPos(&pos);
		ScreenToClient(m_hWND, &pos);
		RECT rcClient = { 0, 0, m_size.cx, m_size.cy };
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			m_texthost.m_ts->OnTxSetCursor(DVASPECT_CONTENT, 0, NULL, NULL, hDC, NULL, &rcClient, pos.x, pos.y);
			ReleaseDC(m_hWND, hDC);
		}
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}

	LRESULT TinyRichText::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}
	LRESULT TinyRichText::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m_hWND, &ps);
		RECT rect;
		GetClientRect(&rect);
		INT oldMode = ::SetGraphicsMode(hDC, GM_COMPATIBLE);
		m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hDC, NULL, reinterpret_cast<LPCRECTL>(&rect), NULL, NULL, NULL, 0, TXTVIEW_ACTIVE);
		::SetGraphicsMode(hDC, oldMode);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}
	LRESULT TinyRichText::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}
	LRESULT TinyRichText::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}
	LRESULT TinyRichText::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return lRes;
		return lRes;
	}
	LRESULT TinyRichText::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		RECT rcClient;
		GetClientRect(&rcClient);
		m_texthost.m_ts->OnTxInPlaceActivate(&rcClient);
		m_texthost.m_ts->OnTxUIActivate();
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}

	LRESULT TinyRichText::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_texthost.m_ts);
		bHandled = TRUE;
		m_texthost.m_ts->OnTxInPlaceDeactivate();
		m_texthost.m_ts->OnTxUIDeactivate();
		LRESULT lRes = 0;
		if (FAILED(m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes)))
			return FALSE;
		return lRes;
	}

	LRESULT TinyRichText::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		m_texthost.Initialize(m_hWND);
		RECT rcClient;
		GetClientRect(&rcClient);
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			LONG xPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX);
			LONG yPerInch = ::GetDeviceCaps(hDC, LOGPIXELSY);
			m_texthost.m_sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
			m_texthost.m_sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);
			ReleaseDC(m_hWND, hDC);
			m_texthost.m_ts->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
		}
		m_texthost.m_ts->TxSetText(L"Windowless RichText");
		return FALSE;
	}
	LRESULT TinyRichText::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}