#include "../stdafx.h"
#include "TinyVisualFrame.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualTextBox.h"
#include "TinyVisualScrollBar.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualTextBox, TinyVisual);

		TinyVisualTextBox::TinyVisualTextBox()
			:m_hscroll(NULL),
			m_vscroll(NULL)
		{

		}
		TinyVisualTextBox::TinyVisualTextBox(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree),
			m_hscroll(NULL),
			m_vscroll(NULL)
		{

		}
		BOOL TinyVisualTextBox::SetReadonly(BOOL fReadOnly)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetReadonly(fReadOnly);
		}
		BOOL TinyVisualTextBox::SetMultiline(BOOL fMultiline)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetMultiline(fMultiline);
		}
		BOOL TinyVisualTextBox::SetPassword(BOOL fPassword, CHAR s)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.SetPassword(fPassword, s);
		}
		BOOL TinyVisualTextBox::ShowScrollBar(INT bar, BOOL fShow)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.ShowScrollBar(bar, fShow);
		}
		BOOL TinyVisualTextBox::SetTextMode(UINT fMode)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETTEXTMODE, (WPARAM)fMode, 0, &lRes);
			return static_cast<BOOL>(lRes);
		}
		UINT TinyVisualTextBox::GetTextMode()
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETTEXTMODE, 0, 0, &lRes);
			return static_cast<UINT>(lRes);
		}
		INT TinyVisualTextBox::GetLineCount()
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINECOUNT, 0, 0, &lRes);
			return static_cast<INT>(lRes);
		}
		TinyPoint TinyVisualTextBox::GetCharPos(LONG lChar)
		{
			ASSERT(m_texthost.m_ts);
			TinyPoint pos;
			m_texthost.m_ts->TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pos, (LPARAM)lChar, NULL);
			return pos;
		}
		INT TinyVisualTextBox::GetLine(INT nIndex, LPTSTR lpszBuffer)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer, &lRes);
			return static_cast<INT>(lRes);
		}
		INT TinyVisualTextBox::GetLine(INT nIndex, LPTSTR lpszBuffer, INT nMaxLength)
		{
			ASSERT(m_texthost.m_ts);
			ASSERT(sizeof(nMaxLength) <= nMaxLength * sizeof(TCHAR) && nMaxLength > 0);
			*(LPINT)lpszBuffer = nMaxLength;
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLINE, (WPARAM)nIndex, (LPARAM)lpszBuffer, &lRes);
			return static_cast<INT>(lRes);
		}
		INT TinyVisualTextBox::LineIndex(INT nLine)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_LINEINDEX, nLine, 0, &lRes);
			return static_cast<INT>(lRes);
		}

		INT TinyVisualTextBox::LineLength(INT nLine)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_LINELENGTH, nLine, 0, &lRes);
			return static_cast<INT>(lRes);
		}
		void TinyVisualTextBox::GetSel(LONG& nStartChar, LONG& nEndChar)
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
			nStartChar = cr.cpMin;
			nEndChar = cr.cpMax;
		}
		void TinyVisualTextBox::GetSel(CHARRANGE &cr)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualTextBox::SetSel(LONG nStartChar, LONG nEndChar)
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			cr.cpMin = nStartChar;
			cr.cpMax = nEndChar;
			m_texthost.m_ts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualTextBox::SetSel(CHARRANGE &cr)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, NULL);
		}
		void TinyVisualTextBox::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
		{
			ASSERT(m_texthost.m_ts);
			wstring sz = StringToWString(lpszNewText);
			m_texthost.m_ts->TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)sz.c_str(), NULL);
		}
		LONG TinyVisualTextBox::LineFromChar(LONG nIndex)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lRes);
			return static_cast<LONG>(lRes);
		}
		TinyPoint TinyVisualTextBox::PosFromChar(UINT nChar)
		{
			ASSERT(m_texthost.m_ts);
			POINTL pos;
			m_texthost.m_ts->TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pos, nChar, NULL);
			return TinyPoint(pos.x, pos.y);
		}
		INT TinyVisualTextBox::CharFromPos(TinyPoint pt)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			POINTL ptl = { pt.x, pt.y };
			m_texthost.m_ts->TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lRes);
			return static_cast<INT>(lRes);
		}
		void TinyVisualTextBox::LimitText(LONG nChars)
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->TxSendMessage(EM_EXLIMITTEXT, 0, nChars, NULL);
		}
		LONG TinyVisualTextBox::GetLimitText()
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETLIMITTEXT, 0, 0L, &lRes);
			return static_cast<INT>(lRes);
		}
		DWORD TinyVisualTextBox::GetDefaultCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualTextBox::GetDefaultCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualTextBox::GetSelectionCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualTextBox::GetSelectionCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualTextBox::GetParaFormat(PARAFORMAT &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		DWORD TinyVisualTextBox::GetParaFormat(PARAFORMAT2 &pf)
		{
			ASSERT(m_texthost.m_ts);;
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<DWORD>(lRes);
		}

		BOOL TinyVisualTextBox::SetParaFormat(PARAFORMAT &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<BOOL>(lRes);
		}
		BOOL TinyVisualTextBox::SetParaFormat(PARAFORMAT2 &pf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			pf.cbSize = sizeof(PARAFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
			return static_cast<BOOL>(lRes);
		}
		INT TinyVisualTextBox::GetTextRange(INT nFirst, INT nLast, wstring& wszText)
		{
			ASSERT(m_texthost.m_ts);
			TEXTRANGEW textRange;
			textRange.chrg.cpMin = nFirst;
			textRange.chrg.cpMax = nLast;
			INT nLength = INT(nLast - nFirst + 1);
			ASSERT(nLength > 0);
			wszText.resize(nLength);
			textRange.lpstrText = &wszText[0];
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange, &lRes);
			return static_cast<INT>(lRes);
		}
		LONG TinyVisualTextBox::GetTextLength()
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(WM_GETTEXTLENGTH, NULL, NULL, &lRes);
			return static_cast<LONG>(lRes);
		}
		LONG TinyVisualTextBox::GetTextLengthEx(DWORD dwFlags, UINT uCodePage)
		{
			ASSERT(m_texthost.m_ts);
			GETTEXTLENGTHEX textLenEx;
			textLenEx.flags = dwFlags;
			if (uCodePage == -1)
			{
#ifdef _UNICODE
				textLenEx.codepage = 1200;
#else
				textLenEx.codepage = CP_ACP;
#endif
			}
			else
			{
				textLenEx.codepage = uCodePage;
			}
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lRes);
			return static_cast<LONG>(lRes);

		}
		LONG TinyVisualTextBox::GetSelText(LPWSTR lpBuf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpBuf, &lRes);
			return static_cast<LONG>(lRes);
		}
		string TinyVisualTextBox::GetSelText()
		{
			ASSERT(m_texthost.m_ts);
			CHARRANGE cr;
			cr.cpMin = cr.cpMax = 0;
			m_texthost.m_ts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
			wstring wszText;
			wszText.resize(cr.cpMax - cr.cpMin + 1);
			m_texthost.m_ts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)wszText.c_str(), NULL);
			string szText = WStringToString(wszText);
			return szText;
		}
		BOOL TinyVisualTextBox::SetDefaultCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualTextBox::SetDefaultCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualTextBox::SetSelectionCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualTextBox::SetSelectionCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualTextBox::SetWordCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		BOOL TinyVisualTextBox::SetWordCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost.m_ts);
			LRESULT lRes = 0;
			cf.cbSize = sizeof(CHARFORMAT2);
			m_texthost.m_ts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
			return static_cast<BOOL>(lRes);
		}

		HRESULT TinyVisualTextBox::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			m_document->SetFocus(this);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualTextBox::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
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
		HRESULT	TinyVisualTextBox::OnFocus(BOOL bFlag)
		{
			ASSERT(m_texthost.m_ts);
			if (bFlag)
			{
				m_texthost.m_ts->OnTxUIActivate();
				m_texthost.m_ts->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
				m_texthost.TxShowCaret(TRUE);
				SetSel(MAKELONG(0, -1), TRUE);
			}
			else
			{
				m_texthost.m_ts->OnTxUIDeactivate();
				m_texthost.m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);
				m_texthost.TxShowCaret(FALSE);
			}
			return FALSE;
		}
		HRESULT TinyVisualTextBox::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes);
		}
		TinyVisualTextBox::~TinyVisualTextBox()
		{

		}
		TinyString TinyVisualTextBox::RetrieveTag() const
		{
			return TinyVisualTag::TEXTBOX;
		}

		void TinyVisualTextBox::SetText(const TinyString& szText)
		{
			if (m_texthost.m_ts != NULL)
			{
				TinyString szTMP = std::move(szText);
				if (!szTMP.IsEmpty())
				{
					wstring val = szTMP.ToWString();
					m_texthost.m_ts->TxSetText(val.c_str());
				}
				else
				{
					m_texthost.m_ts->TxSetText(L"");
				}
			}
			TinyVisual::SetText(szText);
		}
		void TinyVisualTextBox::SetTextColor(COLORREF color)
		{
			m_texthost.SetTextColor(color);
			TinyVisual::SetTextColor(color);
		}
		void TinyVisualTextBox::SetBackgroundColor(COLORREF color)
		{
			m_texthost.SetbackgroundColor(color);
			TinyVisual::SetBackgroundColor(color);
		}
		TinyString TinyVisualTextBox::GetText()
		{
			ASSERT(m_texthost.m_ts);
			LONG size = GetTextLengthEx(GTL_DEFAULT);
			GETTEXTEX gt;
			gt.flags = GT_DEFAULT;
			gt.lpDefaultChar = NULL;
			gt.lpUsedDefChar = NULL;
			string szText;
#ifdef _UNICODE
			gt.cb = sizeof(TCHAR) * (size + 1);
			gt.codepage = 1200;
			szText.resize(gt.cb);
#else
			gt.cb = sizeof(TCHAR) * size * 2 + 1;
			gt.codepage = CP_ACP;
			szText.resize(gt.cb);
#endif
			m_texthost.m_ts->TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)&szText[0], NULL);
			return szText.empty() ? "" : szText.c_str();
		}
		BOOL TinyVisualTextBox::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::MULTILINE.STR()) == 0)
			{
				if (TinyVisualBuilder::GetBool(value))
				{
					this->SetMultiline(TRUE);
					this->ShowScrollBar(SB_HORZ, TRUE);
					this->ShowScrollBar(SB_VERT, TRUE);
				}
				else
				{
					this->SetMultiline(FALSE);
					this->ShowScrollBar(SB_HORZ, FALSE);
					this->ShowScrollBar(SB_VERT, FALSE);
				}
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::READONLY.STR()) == 0)
			{
				this->SetReadonly(TinyVisualBuilder::GetBool(value));
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		BOOL TinyVisualTextBox::OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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
		BOOL TinyVisualTextBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			if (m_texthost.m_ts != NULL)
			{
				TinyClipCanvas canvas(hDC, this, rcPaint);
				::SetGraphicsMode(canvas, GM_COMPATIBLE);
				TinyRectangle clip;
				m_texthost.TxGetClientRect(&clip);
				m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, canvas, NULL, reinterpret_cast<LPCRECTL>(&clip), NULL, reinterpret_cast<LPRECT>(&clip), NULL, 0, 0);
				return TRUE;
			}
			return FALSE;
		}
		HRESULT TinyVisualTextBox::OnCreate()
		{
			m_document->GetVisualHWND()->AddFilter(this);
			m_texthost.Initialize(this);
			return S_OK;
		}
		HRESULT TinyVisualTextBox::OnInitialize()
		{
			TinySize size = this->GetSize();
			m_hscroll = static_cast<TinyVisualHScrollBar*>(m_document->Create(0, size.cy - 12, size.cx, 12, TinyVisualTag::HSCROLLBAR, this));
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualTextBox::OnPosChange));
			m_hscroll->EVENT_PosChange += m_onPosChange;
			m_hscroll->SetVisible(FALSE);
			m_vscroll = static_cast<TinyVisualVScrollBar*>(m_document->Create(size.cx - 12, 0, 12, size.cy, TinyVisualTag::VSCROLLBAR, this));
			m_vscroll->EVENT_PosChange += m_onPosChange;
			m_vscroll->SetVisible(FALSE);
			m_texthost.SetRectangle(GetWindowRect());
			m_texthost.UpdateFormat();
			m_texthost.UpdateView();
			if (!m_szText.IsEmpty())
			{
				m_texthost.m_ts->TxSetText(m_szText.ToWString().c_str());
			}
			return S_OK;
		}
		HRESULT TinyVisualTextBox::OnDestory()
		{
			m_hscroll->EVENT_PosChange -= m_onPosChange;
			m_vscroll->EVENT_PosChange -= m_onPosChange;
			m_document->GetVisualHWND()->RemoveFilter(this);
			return S_OK;
		}
		void TinyVisualTextBox::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			if (m_texthost.m_ts != NULL)
			{
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
}
