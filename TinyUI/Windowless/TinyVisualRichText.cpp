#include "../stdafx.h"
#include "TinyVisualRichText.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

namespace TinyUI
{
	namespace Windowless
	{
		TinyTextHost::TinyTextHost(TinyVisualRichText* spvis)
			:m_cRef(1),
			m_pts(NULL),
			m_spvis(spvis),
			m_hInstance(NULL)
		{
			m_hInstance = LoadLibrary(TEXT("Msftedit.dll"));
			if (m_hInstance != NULL)
			{
				PCreateTextServices ps = (PCreateTextServices)GetProcAddress(m_hInstance, "CreateTextServices");
				if (ps != NULL)
				{
					IUnknown* pUnknown = NULL;
					if (ps(NULL, static_cast<ITextHost*>(this), &pUnknown) == S_OK)
					{
						pUnknown->QueryInterface(IID_ITextServices, (void**)&m_pts);
						pUnknown->Release();
					}
				}
			}
		}
		TinyTextHost::~TinyTextHost()
		{
			SAFE_RELEASE(m_pts);
			if (m_hInstance != NULL)
			{
				FreeLibrary(m_hInstance);
				m_hInstance = NULL;
			}
		}
		HRESULT STDMETHODCALLTYPE TinyTextHost::QueryInterface(REFIID riid, void **ppvObject)
		{
			if (IsEqualIID(riid, IID_ITextHost) || IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = this;
			}
			else
			{
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return NOERROR;
		}
		ULONG STDMETHODCALLTYPE TinyTextHost::AddRef(void)
		{
			return InterlockedIncrement(&this->m_cRef);
		}
		ULONG STDMETHODCALLTYPE TinyTextHost::Release(void)
		{
			if (InterlockedDecrement(&this->m_cRef))
			{
				return this->m_cRef;
			}
			delete this;
			return NOERROR;
		}
		HDC TinyTextHost::TxGetDC()
		{
			return ::GetDC(m_spvis->GetDocument()->GetVisualHWND()->Handle());
		}
		INT TinyTextHost::TxReleaseDC(HDC hdc)
		{
			return ::ReleaseDC(m_spvis->GetDocument()->GetVisualHWND()->Handle(), hdc);
		}
		BOOL TinyTextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
		{
			return FALSE;
		}
		BOOL TinyTextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
		{
			return FALSE;
		}
		BOOL TinyTextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
		{
			return FALSE;
		}
		BOOL TinyTextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
		{
			return FALSE;
		}
		void TinyTextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
		{

		}
		void TinyTextHost::TxViewChange(BOOL fUpdate)
		{

		}
		BOOL TinyTextHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
		{
			ASSERT(m_spvis);
			return ::CreateCaret(m_spvis->GetDocument()->GetVisualHWND()->Handle(), hbmp, xWidth, yHeight);
		}
		BOOL TinyTextHost::TxShowCaret(BOOL fShow)
		{
			ASSERT(m_spvis);
			if (fShow)
				return ::ShowCaret(m_spvis->GetDocument()->GetVisualHWND()->Handle());
			return ::HideCaret(m_spvis->GetDocument()->GetVisualHWND()->Handle());
		}
		BOOL TinyTextHost::TxSetCaretPos(INT x, INT y)
		{
			return ::SetCaretPos(x, y);
		}
		BOOL TinyTextHost::TxSetTimer(UINT idTimer, UINT uTimeout)
		{
			return FALSE;
		}
		void TinyTextHost::TxKillTimer(UINT idTimer)
		{

		}
		void TinyTextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
		{

		}
		void TinyTextHost::TxSetCapture(BOOL fCapture)
		{
			if (fCapture)
			{
				::SetCapture(m_spvis->GetDocument()->GetVisualHWND()->Handle());
			}
			else
			{
				::ReleaseCapture();
			}
		}
		void TinyTextHost::TxSetFocus()
		{
			::SetFocus(m_spvis->GetDocument()->GetVisualHWND()->Handle());
		}
		void TinyTextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
		{
			::SetCursor(hcur);
		}
		BOOL TinyTextHost::TxScreenToClient(LPPOINT lppt)
		{
			ASSERT(m_spvis);
			return ::ScreenToClient(m_spvis->GetDocument()->GetVisualHWND()->Handle(), lppt);
		}
		BOOL TinyTextHost::TxClientToScreen(LPPOINT lppt)
		{
			ASSERT(m_spvis);
			return ::ClientToScreen(m_spvis->GetDocument()->GetVisualHWND()->Handle(), lppt);
		}
		HRESULT TinyTextHost::TxActivate(LONG * plOldState)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxDeactivate(LONG lNewState)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetClientRect(LPRECT prc)
		{
			ASSERT(m_spvis);
			TinyRectangle s = m_spvis->GetDocument()->GetWindowRect(m_spvis);
			CopyRect(prc, &s);
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetViewInset(LPRECT prc)
		{
			memset(prc, 0, sizeof(RECT));
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetCharFormat(const CHARFORMATW **ppCF)
		{
			/*ppCF = &m_cf;*/
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetParaFormat(const PARAFORMAT **ppPF)
		{
			//*ppPF = &m_pf;
			return S_OK;
		}
		COLORREF TinyTextHost::TxGetSysColor(INT nIndex)
		{
			return ::GetSysColor(nIndex);
		}
		HRESULT TinyTextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
		{
			*pstyle = TXTBACK_TRANSPARENT;
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetMaxLength(DWORD *plength)
		{
			//*plength = m_dwLimit;
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetPasswordChar(TCHAR *pch)
		{
			//*pch = m_passwordChar;
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetAcceleratorPos(LONG *pcp)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetExtent(LPSIZEL lpExtent)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::OnTxCharFormatChange(const CHARFORMATW * pCF)
		{
			return S_OK;
		}
		HRESULT TinyTextHost::OnTxParaFormatChange(const PARAFORMAT * pPF)
		{
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
		{
			/*DWORD dwProp = 0;
			*pdwBits = dwProp & dwMask;*/
			return S_OK;
		}
		HRESULT TinyTextHost::TxNotify(DWORD iNotify, void *pv)
		{
			return S_OK;
		}
		HIMC TinyTextHost::TxImmGetContext()
		{
			return NULL;
		}
		void TinyTextHost::TxImmReleaseContext(HIMC himc)
		{

		}
		HRESULT TinyTextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth)
		{
			*lSelBarWidth = 0;
			return S_OK;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}
		LRESULT TinyVisualRichText::OnCreate()
		{
			m_texthost.Reset(new TinyTextHost(this));
			TinyRectangle s = m_document->GetWindowRect(this);
			m_texthost->m_pts->TxSetText(L"²âÊÔ111111");
			return FALSE;
		}
		LRESULT TinyVisualRichText::OnDestory()
		{
			return FALSE;
		}
		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost && m_texthost->m_pts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
			TinyRectangle s = m_document->GetWindowRect(this);
			s.bottom -= 1;
			s.right -= 1;
			canvas.DrawRectangle(s);
			s.left += 2;
			s.top += 2;
			s.bottom -= 3;
			s.right -= 3;
			m_texthost->m_pts->TxDraw(
				DVASPECT_CONTENT,
				0,
				NULL,
				NULL,
				canvas.Handle(),
				NULL,
				(RECTL*)&s,
				NULL,
				(RECT*)&rcPaint,
				NULL,
				NULL,
				TXTVIEW_ACTIVE);
			return FALSE;
		}
		BOOL TinyVisualRichText::CanUndo() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_CANUNDO, 0, 0, &lRes);
		}
		BOOL TinyVisualRichText::CanRedo() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_CANREDO, 0, 0, &lRes);
		}
		UNDONAMEID TinyVisualRichText::GetUndoName() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UNDONAMEID)m_texthost->m_pts->TxSendMessage(EM_GETUNDONAME, 0, 0, &lRes);
		}
		UNDONAMEID TinyVisualRichText::GetRedoName() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UNDONAMEID)m_texthost->m_pts->TxSendMessage(EM_GETREDONAME, 0, 0, &lRes);
		}
		INT TinyVisualRichText::GetLineCount() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_GETLINECOUNT, 0, 0, &lRes);
		}
		BOOL TinyVisualRichText::GetModify() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_GETMODIFY, 0, 0, &lRes);
		}
		void TinyVisualRichText::SetModify(BOOL bModified /* = TRUE */)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_SETMODIFY, bModified, 0, &lRes);
		}
		BOOL TinyVisualRichText::SetTextMode(UINT fMode)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETTEXTMODE, (WPARAM)fMode, 0, &lRes);
		}
		UINT TinyVisualRichText::GetTextMode() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UINT)m_texthost->m_pts->TxSendMessage(EM_GETTEXTMODE, 0, 0, &lRes);
		}
		void TinyVisualRichText::GetRect(LPRECT lpRect) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_GETRECT, 0, (LPARAM)lpRect, &lRes);
		}
		TinyPoint TinyVisualRichText::GetCharPos(LONG lChar) const
		{
			ASSERT(m_texthost);
			TinyPoint pt;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, &lRes);
			return pt;
		}
		UINT TinyVisualRichText::GetOptions() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UINT)m_texthost->m_pts->TxSendMessage(EM_GETOPTIONS, 0, 0, &lRes);
		}
		void TinyVisualRichText::SetOptions(WORD wOp, DWORD dwFlags)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_SETOPTIONS, (WPARAM)wOp, (LPARAM)dwFlags, &lRes);
		}
		BOOL TinyVisualRichText::SetAutoURLDetect(BOOL bEnable /* = TRUE */)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_AUTOURLDETECT, (WPARAM)bEnable, 0, &lRes);
		}
		void TinyVisualRichText::EmptyUndoBuffer()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, &lRes);
		}
		UINT TinyVisualRichText::SetUndoLimit(UINT nLimit)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UINT)m_texthost->m_pts->TxSendMessage(EM_SETUNDOLIMIT, (WPARAM)nLimit, 0, &lRes);
		}
		void TinyVisualRichText::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText, &lRes);
		}
		void TinyVisualRichText::SetRect(LPCRECT lpRect)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_SETRECT, 0, (LPARAM)lpRect, &lRes);
		}
		void TinyVisualRichText::StopGroupTyping()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_STOPGROUPTYPING, 0, 0, &lRes);
		}
		BOOL TinyVisualRichText::Redo()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_REDO, 0, 0, &lRes);
		}
		BOOL TinyVisualRichText::Undo()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_UNDO, 0, 0, &lRes);
		}
		void TinyVisualRichText::Clear()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(WM_CLEAR, 0, 0, &lRes);
		}
		void TinyVisualRichText::Copy()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(WM_COPY, 0, 0, &lRes);
		}
		void TinyVisualRichText::Cut()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(WM_CUT, 0, 0, &lRes);
		}
		void TinyVisualRichText::Paste()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(WM_PASTE, 0, 0, &lRes);
		}
		BOOL TinyVisualRichText::SetReadOnly(BOOL bReadOnly /* = TRUE */)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETREADONLY, bReadOnly, 0L, &lRes);
		}
		INT TinyVisualRichText::GetFirstVisibleLine() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0L, &lRes);
		}
		BOOL TinyVisualRichText::DisplayBand(LPRECT pDisplayRect)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_DISPLAYBAND, 0, (LPARAM)pDisplayRect, &lRes);
		}
		void TinyVisualRichText::GetSel(CHARRANGE &cr) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, &lRes);
		}
		BOOL TinyVisualRichText::GetPunctuation(UINT fType, PUNCTUATION* lpPunc) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_GETPUNCTUATION, (WPARAM)fType, (LPARAM)lpPunc, &lRes);
		}
		BOOL TinyVisualRichText::SetPunctuation(UINT fType, PUNCTUATION* lpPunc)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETPUNCTUATION, (WPARAM)fType, (LPARAM)lpPunc, &lRes);
		}
		void TinyVisualRichText::LimitText(LONG nChars)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXLIMITTEXT, 0, nChars, &lRes);
		}
		LONG TinyVisualRichText::LineFromChar(LONG nIndex) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lRes);
		}
		TinyPoint TinyVisualRichText::PosFromChar(UINT nChar) const
		{
			ASSERT(m_texthost);
			POINTL pt;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, &lRes);
			return TinyPoint(pt.x, pt.y);
		}
		INT TinyVisualRichText::CharFromPos(TinyPoint pt) const
		{
			ASSERT(m_texthost);
			POINTL ptl = { pt.x, pt.y };
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lRes);
		}
		void TinyVisualRichText::SetSel(CHARRANGE &cr)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lRes);
		}
		DWORD TinyVisualRichText::FindWordBreak(UINT nCode, DWORD nStart) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_FINDWORDBREAK, (WPARAM)nCode, (LPARAM)nStart, &lRes);
		}

		LONG TinyVisualRichText::FindText(DWORD dwFlags, FINDTEXTEX* pFindText) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_FINDTEXTEX, dwFlags, (LPARAM)pFindText, &lRes);
		}

		LONG TinyVisualRichText::FormatRange(FORMATRANGE* pfr, BOOL bDisplay)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_FORMATRANGE, (WPARAM)bDisplay, (LPARAM)pfr, &lRes);
		}

		LONG TinyVisualRichText::GetEventMask() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_GETEVENTMASK, 0, 0L, &lRes);
		}

		LONG TinyVisualRichText::GetLimitText() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_GETLIMITTEXT, 0, 0L, &lRes);
		}

		LONG TinyVisualRichText::GetSelText(LPSTR lpBuf) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpBuf, &lRes);
		}

		void TinyVisualRichText::HideSelection(BOOL bHide, BOOL bPerm)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_HIDESELECTION, bHide, bPerm, &lRes);
		}

		void TinyVisualRichText::RequestResize()
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_REQUESTRESIZE, 0, 0L, &lRes);
		}

		WORD TinyVisualRichText::GetSelectionType() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (WORD)m_texthost->m_pts->TxSendMessage(EM_SELECTIONTYPE, 0, 0L, &lRes);
		}

		UINT TinyVisualRichText::GetWordWrapMode() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UINT)m_texthost->m_pts->TxSendMessage(EM_GETWORDWRAPMODE, 0, 0, &lRes);
		}

		UINT TinyVisualRichText::SetWordWrapMode(UINT uFlags) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (UINT)m_texthost->m_pts->TxSendMessage(EM_SETWORDWRAPMODE, (WPARAM)uFlags, 0, &lRes);
		}

		COLORREF TinyVisualRichText::SetBackgroundColor(BOOL bSysColor, COLORREF cr)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (COLORREF)m_texthost->m_pts->TxSendMessage(EM_SETBKGNDCOLOR, bSysColor, cr, &lRes);
		}

		DWORD TinyVisualRichText::SetEventMask(DWORD dwEventMask)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lRes);
		}
		BOOL TinyVisualRichText::SetTargetDevice(HDC hDC, LONG lLineWidth)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETTARGETDEVICE, (WPARAM)hDC, lLineWidth, &lRes);
		}

		LONG TinyVisualRichText::StreamIn(INT nFormat, EDITSTREAM &es)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lRes);
		}

		LONG TinyVisualRichText::StreamOut(INT nFormat, EDITSTREAM &es)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lRes);
		}

		LONG TinyVisualRichText::GetTextLength() const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(WM_GETTEXTLENGTH, NULL, NULL, &lRes);
		}

		INT TinyVisualRichText::GetLine(INT nIndex, LPTSTR lpszBuffer) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer, &lRes);
		}

		INT TinyVisualRichText::LineIndex(INT nLine /* = -1 */) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_LINEINDEX, nLine, 0, &lRes);
		}

		INT TinyVisualRichText::LineLength(INT nLine /* = -1 */) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_LINELENGTH, nLine, 0, &lRes);
		}

		void TinyVisualRichText::LineScroll(INT nLines, INT nChars /* = 0 */)
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_LINESCROLL, nChars, nLines, &lRes);
		}

		void TinyVisualRichText::SetSel(LONG nStartChar, LONG nEndChar)
		{
			ASSERT(m_texthost);
			CHARRANGE cr;
			cr.cpMin = nStartChar;
			cr.cpMax = nEndChar;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lRes);
		}

		BOOL TinyVisualRichText::CanPaste(UINT nFormat) const
		{
			ASSERT(m_texthost);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_CANPASTE, nFormat, 0L, &lRes);
		}

		void TinyVisualRichText::PasteSpecial(UINT nClipFormat, DWORD dvAspect, HMETAFILE hMF)
		{
			ASSERT(m_texthost);
			REPASTESPECIAL reps;
			reps.dwAspect = dvAspect;
			reps.dwParam = (DWORD_PTR)hMF;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_PASTESPECIAL, nClipFormat, (LPARAM)&reps, &lRes);
		}

		INT TinyVisualRichText::GetLine(INT nIndex, _Out_cap_post_count_(nMaxLength, return) LPTSTR lpszBuffer, INT nMaxLength) const
		{
			ASSERT(m_texthost);
			ASSERT(sizeof(nMaxLength) <= nMaxLength*sizeof(TCHAR) && nMaxLength > 0);
			*(LPINT)lpszBuffer = nMaxLength;
			LRESULT lRes = 0;
			return (INT)m_texthost->m_pts->TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer, &lRes);
		}

		void TinyVisualRichText::GetSel(LONG& nStartChar, LONG& nEndChar) const
		{
			ASSERT(m_texthost);
			CHARRANGE cr;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, &lRes);
			nStartChar = cr.cpMin;
			nEndChar = cr.cpMax;
		}

		TinyString TinyVisualRichText::GetSelText() const
		{
			ASSERT(m_texthost);
			CHARRANGE cr;
			cr.cpMin = cr.cpMax = 0;
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, &lRes);
			TinyString strText(cr.cpMax - cr.cpMin + 1);
			m_texthost->m_pts->TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)strText.STR(), &lRes);
			return TinyString(strText);
		}

		LONG TinyVisualRichText::GetTextLengthEx(DWORD dwFlags, UINT nCodePage) const
		{
			ASSERT(m_texthost);
			GETTEXTLENGTHEX textLenEx;
			textLenEx.flags = dwFlags;
			if (nCodePage == -1)
			{
#ifdef _UNICODE
				textLenEx.codepage = 1200;
#else
				textLenEx.codepage = CP_ACP;
#endif
			}
			else
				textLenEx.codepage = nCodePage;
			LRESULT lRes = 0;
			return (LONG)m_texthost->m_pts->TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lRes);
		}

		INT TinyVisualRichText::GetTextRange(INT nFirst, INT nLast, TinyString& refString) const
		{
			ASSERT(m_texthost);

			TEXTRANGE textRange;
			textRange.chrg.cpMin = nFirst;
			textRange.chrg.cpMax = nLast;

			INT nLength = INT(nLast - nFirst + 1);
			ASSERT(nLength > 0);
			textRange.lpstrText = refString.Substring(nFirst, nLength).STR();
			LRESULT lRes = 0;
			nLength = (INT)m_texthost->m_pts->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange, &lRes);
			return (INT)nLength;
		}

		BOOL TinyVisualRichText::SetDefaultCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
		}

		BOOL TinyVisualRichText::SetDefaultCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
		}

		BOOL TinyVisualRichText::SetSelectionCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
		}

		BOOL TinyVisualRichText::SetSelectionCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
		}

		BOOL TinyVisualRichText::SetWordCharFormat(CHARFORMAT &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
		}

		BOOL TinyVisualRichText::SetWordCharFormat(CHARFORMAT2 &cf)
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lRes);
		}

		DWORD TinyVisualRichText::GetDefaultCharFormat(CHARFORMAT &cf) const
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
		}

		DWORD TinyVisualRichText::GetDefaultCharFormat(CHARFORMAT2 &cf) const
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
		}

		DWORD TinyVisualRichText::GetSelectionCharFormat(CHARFORMAT &cf) const
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
		}

		DWORD TinyVisualRichText::GetSelectionCharFormat(CHARFORMAT2 &cf) const
		{
			ASSERT(m_texthost);
			cf.cbSize = sizeof(CHARFORMAT2);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
		}

		DWORD TinyVisualRichText::GetParaFormat(PARAFORMAT &pf) const
		{
			ASSERT(m_texthost);
			pf.cbSize = sizeof(PARAFORMAT);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		}

		DWORD TinyVisualRichText::GetParaFormat(PARAFORMAT2 &pf) const
		{
			ASSERT(m_texthost);
			pf.cbSize = sizeof(PARAFORMAT2);
			LRESULT lRes = 0;
			return (DWORD)m_texthost->m_pts->TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		}

		BOOL TinyVisualRichText::SetParaFormat(PARAFORMAT &pf)
		{
			ASSERT(m_texthost);
			pf.cbSize = sizeof(PARAFORMAT);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		}

		BOOL TinyVisualRichText::SetParaFormat(PARAFORMAT2 &pf)
		{
			ASSERT(m_texthost);
			pf.cbSize = sizeof(PARAFORMAT2);
			LRESULT lRes = 0;
			return (BOOL)m_texthost->m_pts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		}
	}
}
