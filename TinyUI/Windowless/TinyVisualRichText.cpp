#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualRichText.h"
#include "TinyVisualScrollBar.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualRichText, TinyVisual);
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_hscroll(NULL),
			m_vscroll(NULL)
		{
		}
		BOOL TinyVisualRichText::SetReadonly(BOOL fReadOnly)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetReadonly(fReadOnly);
		}
		BOOL TinyVisualRichText::SetMultiline(BOOL fMultiline)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetMultiline(fMultiline);
		}
		BOOL TinyVisualRichText::SetPassword(BOOL fPassword, CHAR s)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetPassword(fPassword, s);
		}
		BOOL TinyVisualRichText::ShowScrollBar(INT bar, BOOL fShow)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.ShowScrollBar(bar, fShow);
		}
		INT TinyVisualRichText::GetLineCount()
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINECOUNT, 0, 0, &lRes);
			return static_cast<INT>(lRes);
		}
		TinyPoint TinyVisualRichText::GetCharPos(LONG lChar)
		{
			ASSERT(m_texthost.m_ts);
			TinyPoint pos;
			m_texthost.m_ts->TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pos, (LPARAM)lChar, NULL);
			return pos;
		}
		INT TinyVisualRichText::GetLine(INT nIndex, LPTSTR lpszBuffer)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer, &lRes);
			return static_cast<INT>(lRes);
		}
		INT TinyVisualRichText::GetLine(INT nIndex, LPTSTR lpszBuffer, INT nMaxLength)
		{
			ASSERT(m_texthost.m_ts);
			ASSERT(sizeof(nMaxLength) <= nMaxLength * sizeof(TCHAR) && nMaxLength > 0);
			*(LPINT)lpszBuffer = nMaxLength;
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINE, (WPARAM)nIndex, (LPARAM)lpszBuffer, &lRes);
			return static_cast<INT>(lRes);
		}
		INT TinyVisualRichText::LineIndex(INT nLine)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_LINEINDEX, nLine, 0, &lRes);
			return static_cast<INT>(lRes);
		}

		INT TinyVisualRichText::LineLength(INT nLine)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_LINELENGTH, nLine, 0, &lRes);
			return static_cast<INT>(lRes);
		}
		void TinyVisualRichText::GetSel(LONG& nStartChar, LONG& nEndChar)
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
			nStartChar = cr.cpMin;
			nEndChar = cr.cpMax;
		}
		void TinyVisualRichText::GetSel(CHARRANGE &cr)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualRichText::SetSel(LONG nStartChar, LONG nEndChar)
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			cr.cpMin = nStartChar;
			cr.cpMax = nEndChar;
			m_texthost.m_ts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualRichText::SetSel(CHARRANGE &cr)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualRichText::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText, NULL);
		}
		DWORD TinyVisualRichText::GetDefaultCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualRichText::GetDefaultCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualRichText::GetSelectionCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualRichText::GetSelectionCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualRichText::GetParaFormat(PARAFORMAT &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualRichText::GetParaFormat(PARAFORMAT2 &pf)
		{
			ASSERT(m_texthost.m_ts);;
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		BOOL TinyVisualRichText::SetParaFormat(PARAFORMAT &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<BOOL>(lRes);
		}
		BOOL TinyVisualRichText::SetParaFormat(PARAFORMAT2 &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<BOOL>(lRes);
		}
		INT TinyVisualRichText::GetTextRange(INT nFirst, INT nLast, TinyString& refString)
		{
			ASSERT(m_texthost.m_ts);
			TEXTRANGE textRange;
			textRange.chrg.cpMin = nFirst;
			textRange.chrg.cpMax = nLast;
			INT nLength = INT(nLast - nFirst + 1);
			ASSERT(nLength > 0);
			textRange.lpstrText = refString.Substring(nFirst, nLength).STR();
			m_texthost.m_ts->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange, reinterpret_cast<LRESULT*>(&nLength));
			return (INT)nLength;
		}
		LONG TinyVisualRichText::GetSelText(LPSTR lpBuf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpBuf, &lRes);
			return static_cast<LONG>(lRes);
		}
		TinyString TinyVisualRichText::GetSelText()
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			cr.cpMin = cr.cpMax = 0;
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
			TinyString strText(cr.cpMax - cr.cpMin + 1);
			m_texthost.m_ts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)strText.STR(), NULL);
			return TinyString(strText);
		}
		BOOL TinyVisualRichText::SetDefaultCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualRichText::SetDefaultCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualRichText::SetSelectionCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualRichText::SetSelectionCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualRichText::SetWordCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualRichText::SetWordCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		HRESULT TinyVisualRichText::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			ASSERT(m_texthost.m_ts);
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			POINT pos;
			GetCursorPos(&pos);
			::ScreenToClient(hWND, &pos);
			HDC hDC = GetDC(hWND);
			if (hDC != NULL)
			{
				TinyRectangle clip = GetWindowRect();
				m_texthost.m_ts->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, hDC, NULL, &clip, pos.x, pos.y);
				ReleaseDC(hWND, hDC);
			}
			return TRUE;
		}
		HRESULT	TinyVisualRichText::OnFocus(BOOL bFlag)
		{
			ASSERT(m_texthost.m_ts);
			if (bFlag)
			{
				m_texthost.m_ts->OnTxUIActivate();
				m_texthost.m_ts->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
			}
			else
			{
				m_texthost.m_ts->OnTxInPlaceDeactivate();
				m_texthost.m_ts->OnTxUIDeactivate();
				m_texthost.m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);
			}
			return FALSE;
		}
		HRESULT TinyVisualRichText::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes);
		}
		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}

		void TinyVisualRichText::SetText(LPCSTR pzText)
		{
			if (m_texthost.m_ts != NULL)
			{
				string szText(pzText);
				wstring val = StringToWString(pzText);
				m_texthost.m_ts->TxSetText(val.c_str());
			}
			TinyVisual::SetText(pzText);
		}

		BOOL TinyVisualRichText::OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
		{
			if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
				return FALSE;
			if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
				return FALSE;
			if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR)
				return FALSE;
			if (uMsg == WM_SETCURSOR)
				return FALSE;
			if (IsFocus())
			{
				return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lResult) == S_OK;
			}
			return FALSE;
		}
		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost.m_ts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			::SetGraphicsMode(canvas, GM_COMPATIBLE);
			TinyRectangle clip;
			m_texthost.TxGetClientRect(&clip);
			m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, canvas, NULL, reinterpret_cast<LPCRECTL>(&clip), NULL, reinterpret_cast<LPRECT>(&clip), NULL, 0, 0);
			return TRUE;
		}
		HRESULT TinyVisualRichText::OnCreate()
		{
			m_document->GetVisualHWND()->AddFilter(this);
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnInitialize()
		{
			TinySize size = this->GetSize();
			m_hscroll = m_document->Create<TinyVisualHScrollBar>(0, size.cy - 12, size.cx, 12, this);
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualRichText::OnPosChange));
			m_hscroll->EVENT_PosChange += m_onPosChange;
			m_hscroll->SetVisible(FALSE);
			m_vscroll = m_document->Create<TinyVisualVScrollBar>(size.cx - 12, 0, 12, size.cy, this);
			m_vscroll->EVENT_PosChange += m_onPosChange;
			m_vscroll->SetVisible(FALSE);
			m_texthost.Initialize(this);
			m_texthost.UpdateView();
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnDestory()
		{
			m_hscroll->EVENT_PosChange -= m_onPosChange;
			m_vscroll->EVENT_PosChange -= m_onPosChange;
			m_document->GetVisualHWND()->RemoveFilter(this);
			return S_OK;
		}
		void TinyVisualRichText::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			ASSERT(m_texthost.m_ts);
			if (bVer)
			{
				LRESULT lRes = 0;
				m_texthost.m_ts->TxSendMessage(WM_VSCROLL, MAKEWPARAM(code, iNewPos), 0, &lRes);
				LONG lPos = 0;
				m_texthost.m_ts->TxGetVScroll(NULL, NULL, &lPos, NULL, NULL);
				if (lPos != m_vscroll->GetScrollPos())
				{
					m_vscroll->SetScrollPos(lPos);
				}
			}
			else
			{
				LRESULT lRes = 0;
				m_texthost.m_ts->TxSendMessage(WM_HSCROLL, MAKEWPARAM(code, iNewPos), 0, &lRes);
				LONG lPos = 0;
				m_texthost.m_ts->TxGetHScroll(NULL, NULL, &lPos, NULL, NULL);
				if (lPos != m_hscroll->GetScrollPos())
				{
					m_hscroll->SetScrollPos(lPos);
				}
			}
		}
	}
}
