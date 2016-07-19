#pragma once
#include <richedit.h>
#include <textserv.h>
#include <imm.h>
#include <tom.h>
#include "TinyVisual.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualUtility.h"

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
		public:
			BOOL SetCharFormat(HFONT hFont,
				COLORREF crTextColor = GetSysColor(COLOR_WINDOWTEXT),
				LONG yOffset = 0);
			BOOL SetParaFormat(WORD wAlignment = PFA_LEFT,
				LONG	dxStartIndent = 0,
				LONG	dxRightIndent = 0,
				LONG	dxOffset = 0,
				WORD	wNumbering = 0);
			BOOL SetRectangle(const TinyRectangle& rectangle);
		private:
			LONG				m_cRef;
			ITextServices		*m_pts;
			TinyVisualRichText* m_spvis;
			HMODULE				m_hDll;
			PARAFORMAT			m_pf;
			CHARFORMATW			m_cf;
			DWORD				m_dwLimit;
			CHAR				m_chPwd;
			TinyRectangle		m_rectangle;
		};
	}
}


