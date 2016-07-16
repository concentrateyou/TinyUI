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
			/************************************************************************/
			/*ITextHost                                                             */
			/************************************************************************/
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
			//绘制接口包括OLE实现
		private:
			LONG				m_cRef;
			ITextServices		*m_pts;
			DWORD				m_dwStyle;
			TinyVisualRichText* m_spvis;
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
		private:
			TinyTextHost* m_pTextHost;
		};
	}
}


