#include "../stdafx.h"
#include "TinyRichText.h"

namespace TinyUI
{
	LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
	{
		return (LONG)MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
	}
	LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
	{
		return (LONG)MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
	}
	LONG DXtoHimetricX(LONG dx, LONG xPerInch)
	{
		return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
	}
	LONG DYtoHimetricY(LONG dy, LONG yPerInch)
	{
		return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
	}
	//////////////////////////////////////////////////////////////////////////
	TinyRichText::TinyRichText()
		:m_cRef(1),
		m_pts(NULL),
		m_hWND(NULL)
	{

	}


	TinyRichText::~TinyRichText()
	{

	}
	HRESULT TinyRichText::Create(HWND hWND)
	{
		HRESULT hRes = S_OK;
		this->m_hWND = hWND;
		IUnknown* pUnknown = NULL;
		hRes = CreateTextServices(NULL, static_cast<ITextHost*>(this), &pUnknown);
		if (hRes == S_OK)
		{
			hRes = pUnknown->QueryInterface(IID_ITextServices, (void**)&m_pts);
			pUnknown->Release();
		}
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE TinyRichText::QueryInterface(REFIID riid, void **ppvObject)
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
	ULONG STDMETHODCALLTYPE TinyRichText::AddRef(void)
	{
		return InterlockedIncrement(&this->m_cRef);
	}
	ULONG STDMETHODCALLTYPE TinyRichText::Release(void)
	{
		if (InterlockedDecrement(&this->m_cRef))
		{
			return this->m_cRef;
		}
		delete this;
		return NOERROR;
	}
	HDC TinyRichText::TxGetDC()
	{
		return ::GetDC(m_hWND);
	}

	INT TinyRichText::TxReleaseDC(HDC hdc)
	{
		return ::ReleaseDC(m_hWND, hdc);
	}

	BOOL TinyRichText::TxShowScrollBar(INT fnBar, BOOL fShow)
	{
		return FALSE;
	}

	BOOL TinyRichText::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
	{
		return FALSE;
	}

	BOOL TinyRichText::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
	{
		return FALSE;
	}

	BOOL TinyRichText::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
	{
		return FALSE;
	}

	void TinyRichText::TxInvalidateRect(LPCRECT prc, BOOL fMode)
	{

	}

	void TinyRichText::TxViewChange(BOOL fUpdate)
	{

	}

	BOOL TinyRichText::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
	{
		return ::CreateCaret(m_hWND, hbmp, xWidth, yHeight);
	}

	BOOL TinyRichText::TxShowCaret(BOOL fShow)
	{
		if (fShow)
			return ::ShowCaret(m_hWND);
		return ::HideCaret(m_hWND);
	}

	BOOL TinyRichText::TxSetCaretPos(INT x, INT y)
	{
		return ::SetCaretPos(x, y);
	}

	BOOL TinyRichText::TxSetTimer(UINT idTimer, UINT uTimeout)
	{
		return FALSE;
	}

	void TinyRichText::TxKillTimer(UINT idTimer)
	{

	}

	void TinyRichText::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
	{

	}

	void TinyRichText::TxSetCapture(BOOL fCapture)
	{
	}

	void TinyRichText::TxSetFocus()
	{

	}

	void TinyRichText::TxSetCursor(HCURSOR hcur, BOOL fText)
	{

	}

	BOOL TinyRichText::TxScreenToClient(LPPOINT lppt)
	{
		return FALSE;
	}

	BOOL TinyRichText::TxClientToScreen(LPPOINT lppt)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxActivate(LONG * plOldState)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxDeactivate(LONG lNewState)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetClientRect(LPRECT prc)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetViewInset(LPRECT prc)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetCharFormat(const CHARFORMATW **ppCF)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetParaFormat(const PARAFORMAT **ppPF)
	{
		return FALSE;
	}

	COLORREF TinyRichText::TxGetSysColor(INT nIndex)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetBackStyle(TXTBACKSTYLE *pstyle)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetMaxLength(DWORD *plength)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetScrollBars(DWORD *pdwScrollBar)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetPasswordChar(TCHAR *pch)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetAcceleratorPos(LONG *pcp)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetExtent(LPSIZEL lpExtent)
	{
		return FALSE;
	}

	HRESULT TinyRichText::OnTxCharFormatChange(const CHARFORMATW * pCF)
	{
		return FALSE;
	}

	HRESULT TinyRichText::OnTxParaFormatChange(const PARAFORMAT * pPF)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxNotify(DWORD iNotify, void *pv)
	{
		return FALSE;
	}

	HIMC TinyRichText::TxImmGetContext()
	{
		return NULL;
	}

	void TinyRichText::TxImmReleaseContext(HIMC himc)
	{

	}

	HRESULT TinyRichText::TxGetSelectionBarWidth(LONG *lSelBarWidth)
	{
		return FALSE;
	}

	BOOL TinyRichText::TxIsDoubleClickPending()
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetWindow(HWND *phwnd)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxSetForegroundWindow()
	{
		return FALSE;
	}

	HPALETTE TinyRichText::TxGetPalette()
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetEastAsianFlags(LONG *pFlags)
	{
		return FALSE;
	}

	HCURSOR TinyRichText::TxSetCursor2(HCURSOR hcur, BOOL bText)
	{
		return FALSE;
	}

	void TinyRichText::TxFreeTextServicesNotification()
	{

	}

	HRESULT TinyRichText::TxGetEditStyle(DWORD dwItem, DWORD *pdwData)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetWindowStyles(DWORD *pdwStyle, DWORD *pdwExStyle)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxShowDropCaret(BOOL fShow, HDC hdc, LPCRECT prc)
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxDestroyCaret()
	{
		return FALSE;
	}

	HRESULT TinyRichText::TxGetHorzExtent(LONG *plHorzExtent)
	{
		return FALSE;
	}

}
