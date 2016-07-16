#include "../stdafx.h"
#include "TinyVisualRichText.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyTextHost::TinyTextHost(TinyVisualRichText* spvis)
			:m_cRef(1),
			m_pts(NULL),
			m_spvis(spvis)
		{
			HINSTANCE hInstance = LoadLibrary(TEXT("Msftedit.dll"));
			if (hInstance != NULL)
			{
				PCreateTextServices ps = (PCreateTextServices)GetProcAddress(hInstance, "CreateTextServices");
				if (ps != NULL)
				{
					IUnknown* pUnknown = NULL;
					if (ps(NULL, static_cast<ITextHost*>, &pUnknown) == S_OK)
					{
						pUnknown->QueryInterface(IID_ITextServices, (void**)&m_pts);
						pUnknown->Release();
					}
				}
				FreeLibrary(hInstance);
				hInstance = NULL;
			}
		}
		TinyTextHost::~TinyTextHost()
		{
			SAFE_RELEASE(m_pts);
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
		}
		void TinyTextHost::TxSetFocus()
		{

		}
		void TinyTextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
		{

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
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetCharFormat(const CHARFORMATW **ppCF)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetParaFormat(const PARAFORMAT **ppPF)
		{
			return FALSE;
		}
		COLORREF TinyTextHost::TxGetSysColor(INT nIndex)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetMaxLength(DWORD *plength)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetPasswordChar(TCHAR *pch)
		{
			return FALSE;
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
			return FALSE;
		}
		HRESULT TinyTextHost::OnTxParaFormatChange(const PARAFORMAT * pPF)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxNotify(DWORD iNotify, void *pv)
		{
			return FALSE;
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
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document),
			m_pTextHost(NULL)
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
			m_pTextHost = new TinyTextHost(this);
			TinyRectangle s = m_document->GetWindowRect(this);
			m_pTextHost->m_pts->OnTxInPlaceActivate(&s);
			m_pTextHost->m_pts->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
			return FALSE;
		}
		LRESULT TinyVisualRichText::OnDestory()
		{
			return FALSE;
		}
		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyRectangle s = m_document->GetWindowRect(this);
			m_pTextHost->m_pts->TxDraw(
				DVASPECT_CONTENT,
				0,
				NULL,
				NULL,
				hDC,
				NULL,
				(RECTL*)&s,
				NULL,
				(RECT*)&rcPaint,
				NULL,
				NULL,
				0);
			return TRUE;
		}
	}
}
