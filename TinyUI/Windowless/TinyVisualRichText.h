#pragma once
#include <richedit.h>
#include <textserv.h>
#include <imm.h>
#include <tom.h>
#include "TinyUtility.h"
#include "TinyVisual.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"


namespace TinyUI
{
	namespace Windowless
	{
		extern "C"{
			const IID IID_ITextServices = { 0x8d33f740, 0xcf58, 0x11ce, { 0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 } };
			const IID IID_ITextHost = { 0x03bc25ff, 0x69bb, 0x25ff, { 0x14, 0x0c, 0xbb, 0x69, 0xff, 0x25, 0x24, 0x0c } };
		}
#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif
#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif
		class TinyVisualRichText;
		/// <summary>
		/// 无窗口富文本渲染
		/// </summary>
		class TinyTextHost :public ITextHost
		{
			friend class TinyVisualRichText;
		public:
			TinyTextHost(TinyVisualRichText* spvis);
			~TinyTextHost();
		public:
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
			virtual ULONG STDMETHODCALLTYPE AddRef(void);
			virtual ULONG STDMETHODCALLTYPE Release(void);
			virtual HDC TxGetDC();
			virtual INT TxReleaseDC(HDC hdc);
			virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
			virtual BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
			virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
			virtual BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
			virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
			virtual void TxViewChange(BOOL fUpdate);
			virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
			virtual BOOL TxShowCaret(BOOL fShow);
			virtual BOOL TxSetCaretPos(INT x, INT y);
			virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
			virtual void TxKillTimer(UINT idTimer);
			virtual void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
			virtual void TxSetCapture(BOOL fCapture);
			virtual void TxSetFocus();
			virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
			virtual BOOL TxScreenToClient(LPPOINT lppt);
			virtual BOOL TxClientToScreen(LPPOINT lppt);
			virtual HRESULT TxActivate(LONG * plOldState);
			virtual HRESULT TxDeactivate(LONG lNewState);
			virtual HRESULT TxGetClientRect(LPRECT prc);
			virtual HRESULT TxGetViewInset(LPRECT prc);
			virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
			virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
			virtual COLORREF TxGetSysColor(INT nIndex);
			virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
			virtual HRESULT TxGetMaxLength(DWORD *plength);
			virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
			virtual HRESULT TxGetPasswordChar(TCHAR *pch);
			virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
			virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
			virtual HRESULT OnTxCharFormatChange(const CHARFORMATW * pCF);
			virtual HRESULT OnTxParaFormatChange(const PARAFORMAT * pPF);
			virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
			virtual HRESULT TxNotify(DWORD iNotify, void *pv);
			virtual HIMC TxImmGetContext();
			virtual void TxImmReleaseContext(HIMC himc);
			virtual HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth);
		private:
			LONG				m_cRef;
			ITextServices		*m_pts;
			TinyVisualRichText* m_spvis;
			HINSTANCE			m_hInstance;
		};
		/// <summary>
		/// 无窗口富文本控件
		/// </summary>
		class TinyVisualRichText : public TinyVisual
		{
			friend class TinyTextHost;
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualRichText);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualRichText);
		protected:
			TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualRichText();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			LRESULT OnCreate() OVERRIDE;
			LRESULT OnDestory() OVERRIDE;
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		public:
			BOOL CanUndo() const;
			BOOL CanRedo() const;
			UNDONAMEID GetUndoName() const;
			UNDONAMEID GetRedoName() const;
			INT GetLineCount() const;
			BOOL GetModify() const;
			void SetModify(BOOL bModified = TRUE);
			BOOL SetTextMode(UINT fMode);
			UINT GetTextMode() const;
			void GetRect(LPRECT lpRect) const;
			TinyPoint GetCharPos(LONG lChar) const;
			UINT GetOptions() const;
			void SetOptions(WORD wOp, DWORD dwFlags);
			BOOL SetAutoURLDetect(BOOL bEnable = TRUE);
			UINT GetWordWrapMode() const;
			UINT SetWordWrapMode(UINT uFlags) const;
			BOOL GetPunctuation(UINT fType, PUNCTUATION* lpPunc) const;
			BOOL SetPunctuation(UINT fType, PUNCTUATION* lpPunc);
			INT GetLine(INT nIndex, LPTSTR lpszBuffer) const;
			INT GetLine(INT nIndex, _Out_cap_post_count_(nMaxLength, return) LPTSTR lpszBuffer, INT nMaxLength) const;
			BOOL CanPaste(UINT nFormat = 0) const;
			void GetSel(LONG& nStartChar, LONG& nEndChar) const;
			void GetSel(CHARRANGE &cr) const;
			void LimitText(LONG nChars = 0);
			LONG LineFromChar(LONG nIndex) const;
			TinyPoint PosFromChar(UINT nChar) const;
			INT CharFromPos(TinyPoint pt) const;
			void SetSel(LONG nStartChar, LONG nEndChar);
			void SetSel(CHARRANGE &cr);
			DWORD GetDefaultCharFormat(CHARFORMAT &cf) const;
			DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
			DWORD GetSelectionCharFormat(CHARFORMAT &cf) const;
			DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
			LONG GetEventMask() const;
			LONG GetLimitText() const;
			DWORD GetParaFormat(PARAFORMAT &pf) const;
			DWORD GetParaFormat(PARAFORMAT2 &pf) const;
			INT GetTextRange(INT nFirst, INT nLast, TinyString& refString) const;
			LONG GetSelText(LPSTR lpBuf) const;
			TinyString GetSelText() const;
			WORD GetSelectionType() const;
			COLORREF SetBackgroundColor(BOOL bSysColor, COLORREF cr);
			BOOL SetDefaultCharFormat(CHARFORMAT &cf);
			BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
			BOOL SetSelectionCharFormat(CHARFORMAT &cf);
			BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
			BOOL SetWordCharFormat(CHARFORMAT &cf);
			BOOL SetWordCharFormat(CHARFORMAT2 &cf);
			DWORD SetEventMask(DWORD dwEventMask);
			BOOL SetParaFormat(PARAFORMAT &pf);
			BOOL SetParaFormat(PARAFORMAT2 &pf);
			BOOL SetTargetDevice(HDC hDC, LONG lLineWidth);
			LONG GetTextLength() const;
			LONG GetTextLengthEx(DWORD dwFlags, UINT uCodePage = -1) const;
			BOOL SetReadOnly(BOOL bReadOnly = TRUE);
			INT GetFirstVisibleLine() const;
			void EmptyUndoBuffer();
			void StopGroupTyping();
			UINT SetUndoLimit(UINT nLimit);
			INT LineIndex(INT nLine = -1) const;
			INT LineLength(INT nLine = -1) const;
			void LineScroll(INT nLines, INT nChars = 0);
			void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
			void SetRect(LPCRECT lpRect);
			BOOL DisplayBand(LPRECT pDisplayRect);
			LONG FindText(DWORD dwFlags, FINDTEXTEX* pFindText) const;
			DWORD FindWordBreak(UINT nCode, DWORD nStart) const;
			LONG FormatRange(FORMATRANGE* pfr, BOOL bDisplay = TRUE);
			void HideSelection(BOOL bHide, BOOL bPerm);
			void PasteSpecial(UINT nClipFormat, DWORD dvAspect = 0, HMETAFILE hMF = 0);
			void RequestResize();
			LONG StreamIn(INT nFormat, EDITSTREAM &es);
			LONG StreamOut(INT nFormat, EDITSTREAM &es);
			BOOL Undo();
			BOOL Redo();
			void Clear();
			void Copy();
			void Cut();
			void Paste();
		private:
			TinyScopedPtr<TinyTextHost> m_texthost;
		};
	}
}


