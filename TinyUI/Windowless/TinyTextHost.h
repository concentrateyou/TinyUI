#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include <tom.h>
#include "../Control/TinyControl.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
		typedef HRESULT(WINAPI * PCreateTextServices)(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk);
		typedef HRESULT(WINAPI * PShutdownTextServices)(IUnknown *pTextServices);
		class TinyTextHost;
		class TinyRichText;
		//////////////////////////////////////////////////////////////////////////
		class TinyTextHost :public ITextHost
		{
			friend class TinyVisualRichText;
		public:
			TinyTextHost();
			~TinyTextHost();
		public:
			BOOL Initialize(TinyVisualRichText* spvis);
			BOOL UpdateView();
		public:
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
			ULONG STDMETHODCALLTYPE AddRef(void);
			ULONG STDMETHODCALLTYPE Release(void);
			HDC TxGetDC() OVERRIDE;
			INT TxReleaseDC(HDC hdc) OVERRIDE;
			BOOL TxShowScrollBar(INT fnBar, BOOL fShow) OVERRIDE;
			BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) OVERRIDE;
			BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) OVERRIDE;
			BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) OVERRIDE;
			void TxInvalidateRect(LPCRECT prc, BOOL fMode) OVERRIDE;
			void TxViewChange(BOOL fUpdate) OVERRIDE;
			BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) OVERRIDE;
			BOOL TxShowCaret(BOOL fShow) OVERRIDE;
			BOOL TxSetCaretPos(INT x, INT y) OVERRIDE;
			BOOL TxSetTimer(UINT idTimer, UINT uTimeout) OVERRIDE;
			void TxKillTimer(UINT idTimer) OVERRIDE;
			void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) OVERRIDE;
			void TxSetCapture(BOOL fCapture) OVERRIDE;
			void TxSetFocus() OVERRIDE;
			void TxSetCursor(HCURSOR hcur, BOOL fText) OVERRIDE;
			BOOL TxScreenToClient(LPPOINT lppt) OVERRIDE;
			BOOL TxClientToScreen(LPPOINT lppt) OVERRIDE;
			HRESULT TxActivate(LONG * plOldState) OVERRIDE;
			HRESULT TxDeactivate(LONG lNewState) OVERRIDE;
			HRESULT TxGetClientRect(LPRECT prc) OVERRIDE;
			HRESULT TxGetViewInset(LPRECT prc) OVERRIDE;
			HRESULT TxGetCharFormat(const CHARFORMATW **ppCF) OVERRIDE;
			HRESULT TxGetParaFormat(const PARAFORMAT **ppPF) OVERRIDE;
			COLORREF TxGetSysColor(int nIndex) OVERRIDE;
			HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle) OVERRIDE;
			HRESULT TxGetMaxLength(DWORD *plength) OVERRIDE;
			HRESULT TxGetScrollBars(DWORD *pdwScrollBar) OVERRIDE;
			HRESULT TxGetPasswordChar(_Out_ TCHAR *pch) OVERRIDE;
			HRESULT TxGetAcceleratorPos(LONG *pcp) OVERRIDE;
			HRESULT TxGetExtent(LPSIZEL lpExtent) OVERRIDE;
			HRESULT OnTxCharFormatChange(const CHARFORMATW * pCF) OVERRIDE;
			HRESULT OnTxParaFormatChange(const PARAFORMAT * pPF) OVERRIDE;
			HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) OVERRIDE;
			HRESULT TxNotify(DWORD iNotify, void *pv) OVERRIDE;
			HIMC TxImmGetContext() OVERRIDE;
			void TxImmReleaseContext(HIMC himc) OVERRIDE;
			HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth) OVERRIDE;
		private:
			TinyComPtr<ITextServices>	m_ts;
			HINSTANCE					m_hInstance;
			LONG						m_cRef;
			TinyVisualRichText*			m_spvis;
			TinySize					m_extent;
			LONG						m_logpixelsx;
			LONG						m_logpixelsy;
			CHARFORMATW					m_cf;
			PARAFORMAT					m_pf;
			TinyRectangle				m_rectangle;
		};
	}
}



