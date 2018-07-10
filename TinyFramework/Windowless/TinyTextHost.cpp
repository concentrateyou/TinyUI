#include "../stdafx.h"
#include "TinyTextHost.h"
#include "TinyVisualDocument.h"
#include "TinyVisualTextBox.h"
#pragma comment(lib,"imm32.lib")

namespace TinyFramework
{
	namespace Windowless
	{
		EXTERN_C const IID IID_ITextServices =
		{
			0x8d33f740,
			0xcf58,
			0x11ce,
			{ 0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
		};

		EXTERN_C const IID IID_ITextHost =
		{
			0xc5bdd8d0,
			0xd26e,
			0x11ce,
			{ 0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
		};
		//////////////////////////////////////////////////////////////////////////
		TinyTextHost::TinyTextHost()
			:m_hInstance(NULL),
			m_password('*'),
			m_dwStyle(0),
			m_limit(-1)
		{

		}
		TinyTextHost::~TinyTextHost()
		{
			m_ts->OnTxInPlaceDeactivate();
			m_ts->OnTxUIDeactivate();
			m_ts.Release();
			if (m_hInstance)
			{
				::FreeLibrary(m_hInstance);
				m_hInstance = NULL;
			}
		}
		BOOL TinyTextHost::Initialize(TinyVisualTextBox* spvis)
		{
			ASSERT(spvis);
			m_spvis = spvis;
			m_hInstance = ::LoadLibrary(TEXT("Msftedit.dll"));
			if (m_hInstance != NULL)
			{
				PCreateTextServices ps = (PCreateTextServices)::GetProcAddress(m_hInstance, "CreateTextServices");
				if (ps != NULL)
				{
					IUnknown* pUnknown = NULL;
					if (ps(NULL, static_cast<ITextHost*>(this), &pUnknown) == S_OK)
					{
						pUnknown->QueryInterface(IID_ITextServices, (void**)&m_ts);
						pUnknown->Release();
						m_ts->OnTxInPlaceActivate(NULL);
						m_ts->OnTxInPlaceDeactivate();
						m_ts->OnTxUIDeactivate();
						m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
						LRESULT lResult = 0;
						m_ts->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
						HDC hDC = GetDC(m_spvis->m_document->Handle());
						m_logpixelsx = ::GetDeviceCaps(hDC, LOGPIXELSX);
						m_logpixelsy = ::GetDeviceCaps(hDC, LOGPIXELSY);
						ReleaseDC(m_spvis->m_document->Handle(), hDC);
						return TRUE;
					}
				}
			}
			return FALSE;
		}
		BOOL TinyTextHost::SetbackgroundColor(COLORREF cf)
		{
			ASSERT(m_ts || m_spvis);
			m_cf.dwEffects &= ~CFE_AUTOCOLOR;
			m_cf.crBackColor = cf;
			if (FAILED(m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&m_cf, 0)))
				return FALSE;
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::SetTextColor(COLORREF cf)
		{
			ASSERT(m_ts || m_spvis);
			m_cf.dwEffects &= ~CFE_AUTOCOLOR;
			m_cf.crTextColor = cf;
			if (FAILED(m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&m_cf, 0)))
				return FALSE;
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::SetTransparent(BOOL bTransparent)
		{
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, bTransparent ? TXTBIT_BACKSTYLECHANGE : 0)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::SetEnable(BOOL bEnable)
		{
			m_cf.dwEffects |= CFE_DISABLED;
			if (!bEnable)
			{
				m_cf.dwEffects &= ~CFE_DISABLED;
			}
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::SetFont(HFONT hFONT, COLORREF cf)
		{
			ASSERT(m_ts);
			LOGFONT lf;
			GetObject(hFONT, sizeof(LOGFONT), &lf);
			m_cf.cbSize = sizeof(CHARFORMAT2W);
			m_cf.yHeight = lf.lfHeight * LY_PER_INCH / m_logpixelsy;
			m_cf.yOffset = 0;
			m_cf.crTextColor = cf;
			m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
			m_cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK);
			if (lf.lfWeight < FW_BOLD)
				m_cf.dwEffects &= ~CFE_BOLD;
			if (!lf.lfItalic)
				m_cf.dwEffects &= ~CFE_ITALIC;
			if (!lf.lfUnderline)
				m_cf.dwEffects &= ~CFE_UNDERLINE;
			if (!lf.lfStrikeOut)
				m_cf.dwEffects &= ~CFE_STRIKEOUT;
			m_cf.dwMask = CFM_ALL | CFM_BACKCOLOR;
			m_cf.bCharSet = lf.lfCharSet;
			m_cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef UNICODE
			StrCpy(m_cf.szFaceName, lf.lfFaceName);
#else
			MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, m_cf.szFaceName, LF_FACESIZE);
#endif
			ZeroMemory(&m_pf, sizeof(PARAFORMAT2));
			m_pf.cbSize = sizeof(PARAFORMAT2);
			m_pf.dwMask = PFM_ALL;
			m_pf.wAlignment = PFA_LEFT;
			m_pf.cTabCount = 1;
			m_pf.rgxTabs[0] = lDefaultTab;
			if (FAILED(m_ts->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&m_cf, 0)))
				return FALSE;
			if (FAILED(m_ts->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&m_pf, 0)))
				return FALSE;
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE)))
				return FALSE;
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::SetReadonly(BOOL bReadOnly)
		{
			if (bReadOnly)
			{
				m_dwStyle |= ES_READONLY;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_READONLY, TXTBIT_READONLY) == S_OK;
			}
			else
			{
				m_dwStyle &= ~ES_READONLY;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_READONLY, 0) == S_OK;
			}
		}
		BOOL TinyTextHost::SetMultiline(BOOL bMultiline)
		{
			if (bMultiline)
			{
				m_dwStyle |= ES_MULTILINE;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_MULTILINE, TXTBIT_MULTILINE) == S_OK;
			}
			else
			{
				m_dwStyle &= ~ES_MULTILINE;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_MULTILINE, 0) == S_OK;
			}
		}
		BOOL TinyTextHost::SetPassword(BOOL bPassword, CHAR s)
		{
			if (bPassword)
			{
				m_dwStyle |= ES_PASSWORD;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD) == S_OK;
			}
			else
			{
				m_dwStyle &= ~ES_PASSWORD;
				return m_ts->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 0) == S_OK;
			}
		}
		BOOL TinyTextHost::SetWordWrap(BOOL bWarp)
		{
			return m_ts->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, bWarp ? TXTBIT_WORDWRAP : 0) == S_OK;
		}
		BOOL TinyTextHost::SetLimit(LONG limit)
		{
			m_limit = limit;
			return m_ts->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE) == S_OK;
		}
		BOOL TinyTextHost::SetRectangle(const TinyRectangle& rectangle)
		{
			m_rectangle = rectangle;
			m_extent.cx = MAP_PIX_TO_LOGHIM(m_rectangle.Width(), m_logpixelsx);
			m_extent.cy = MAP_PIX_TO_LOGHIM(m_rectangle.Height(), m_logpixelsy);
			if (FAILED(m_ts->OnTxInPlaceActivate(&m_rectangle)))
				return FALSE;
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE | TXTBIT_EXTENTCHANGE, TXTBIT_CLIENTRECTCHANGE | TXTBIT_EXTENTCHANGE)))
				return FALSE;
			return TRUE;
		}
		BOOL TinyTextHost::ShowScrollBar(INT bar, BOOL fShow)
		{
			switch (bar)
			{
			case SB_VERT:
			{
				if (fShow)
				{
					m_dwStyle |= ES_AUTOVSCROLL | WS_VSCROLL;
				}
				else
				{
					m_dwStyle &= ~ES_AUTOVSCROLL;
					m_dwStyle &= ~WS_VSCROLL;
				}
			}
			break;
			case SB_HORZ:
			{
				if (fShow)
				{
					m_dwStyle |= ES_AUTOHSCROLL | WS_HSCROLL;
				}
				else
				{
					m_dwStyle &= ~ES_AUTOHSCROLL;
					m_dwStyle &= ~WS_HSCROLL;
				}
			}
			break;
			}
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE)))
				return FALSE;
			return TRUE;
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
			ASSERT(m_spvis);
			return ::GetDC(m_spvis->m_document->Handle());
		}

		INT TinyTextHost::TxReleaseDC(HDC hdc)
		{
			ASSERT(m_spvis);
			return ::ReleaseDC(m_spvis->m_document->Handle(), hdc);
		}

		BOOL TinyTextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
		{
			ASSERT(m_ts || m_spvis);
			ASSERT(m_spvis->m_vscroll && m_spvis->m_hscroll);
			switch (fnBar)
			{
			case SB_VERT:
			{
				m_spvis->m_vscroll->SetVisible(fShow);
			}
			break;
			case SB_HORZ:
			{
				m_spvis->m_hscroll->SetVisible(fShow);
			}
			break;
			}
			m_rectangle = m_spvis->GetWindowRect();
			if (m_spvis->m_vscroll->IsVisible() && !m_spvis->m_hscroll->IsVisible())
			{
				m_rectangle.right -= m_spvis->m_vscroll->GetSize().cx;
			}
			if (!m_spvis->m_vscroll->IsVisible() && m_spvis->m_hscroll->IsVisible())
			{
				m_rectangle.bottom -= m_spvis->m_hscroll->GetSize().cy;
			}
			if (m_spvis->m_vscroll->IsVisible() && m_spvis->m_hscroll->IsVisible())
			{
				m_rectangle.right -= m_spvis->m_vscroll->GetSize().cx;
				m_rectangle.bottom -= m_spvis->m_hscroll->GetSize().cy;
				TinySize hsize = m_spvis->m_hscroll->GetSize();
				TinySize vsize = m_spvis->m_vscroll->GetSize();
				hsize.cx -= vsize.cx;
				vsize.cy -= hsize.cy;
				m_spvis->m_hscroll->SetSize(hsize);
				m_spvis->m_vscroll->SetSize(vsize);
			}
			if (FAILED(m_ts->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_SCROLLBARCHANGE)))
				return FALSE;
			return TRUE;
		}

		BOOL TinyTextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
		{
			return TRUE;
		}

		BOOL TinyTextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
		{
			ASSERT(m_ts || m_spvis);
			switch (fnBar)
			{
			case SB_VERT:
			{
				LONG lPage = 0;
				m_ts->TxGetVScroll(NULL, NULL, NULL, &lPage, NULL);
				m_spvis->m_vscroll->SetScrollInfo(nMinPos, nMaxPos, lPage, m_spvis->m_vscroll->GetScrollPos());
			}
			break;
			case SB_HORZ:
			{
				LONG lMin = 0;
				LONG lPage = 0;
				m_ts->TxGetHScroll(NULL, NULL, NULL, &lPage, NULL);
				m_spvis->m_hscroll->SetScrollInfo(nMinPos, nMaxPos, lPage, m_spvis->m_hscroll->GetScrollPos());
			}
			break;
			}
			return S_OK;
		}

		BOOL TinyTextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
		{
			ASSERT(m_ts || m_spvis);
			switch (fnBar)
			{
			case SB_VERT:
				m_spvis->m_vscroll->SetScrollPos(nPos);
				break;
			case SB_HORZ:
				m_spvis->m_hscroll->SetScrollPos(nPos);
				break;
			}
			return S_OK;
		}

		void TinyTextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
		{
			ASSERT(m_spvis);
			::InvalidateRect(m_spvis->m_document->Handle(), prc, fMode);
		}

		void TinyTextHost::TxViewChange(BOOL fUpdate)
		{
			ASSERT(m_spvis);
			::UpdateWindow(m_spvis->m_document->Handle());
		}

		BOOL TinyTextHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
		{
			ASSERT(m_spvis);
			return ::CreateCaret(m_spvis->m_document->Handle(), hbmp, xWidth, yHeight);
		}

		BOOL TinyTextHost::TxShowCaret(BOOL fShow)
		{
			ASSERT(m_spvis);
			if (fShow)
			{
				return ::ShowCaret(m_spvis->m_document->Handle());
			}
			else
			{
				return ::HideCaret(m_spvis->m_document->Handle());
			}
		}

		BOOL TinyTextHost::TxSetCaretPos(INT x, INT y)
		{
			return ::SetCaretPos(x, y);
		}

		BOOL TinyTextHost::TxSetTimer(UINT idTimer, UINT uTimeout)
		{
			ASSERT(m_spvis);
			return ::SetTimer(m_spvis->m_document->Handle(), idTimer, uTimeout, NULL);
		}

		void TinyTextHost::TxKillTimer(UINT idTimer)
		{
			ASSERT(m_spvis);
			::KillTimer(m_spvis->m_document->Handle(), idTimer);
		}

		void TinyTextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
		{
			ASSERT(m_spvis);
			m_spvis->Invalidate();
		}

		void TinyTextHost::TxSetCapture(BOOL fCapture)
		{
			ASSERT(m_spvis);
			if (fCapture)
			{
				m_spvis->GetDocument()->SetCapture(m_spvis);
			}
			else
			{
				m_spvis->GetDocument()->ReleaseCapture();
			}
		}

		void TinyTextHost::TxSetFocus()
		{
			ASSERT(m_spvis);
			m_spvis->GetDocument()->SetFocus(m_spvis);
			m_ts->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
		}

		void TinyTextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
		{
			::SetCursor(hcur);
		}

		BOOL TinyTextHost::TxScreenToClient(LPPOINT lppt)
		{
			return ::ScreenToClient(m_spvis->m_document->Handle(), lppt);
		}

		BOOL TinyTextHost::TxClientToScreen(LPPOINT lppt)
		{
			return ::ClientToScreen(m_spvis->m_document->Handle(), lppt);
		}

		HRESULT TinyTextHost::TxActivate(LONG * plOldState)
		{
			return S_OK;
		}

		HRESULT TinyTextHost::TxDeactivate(LONG lNewState)
		{
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetClientRect(LPRECT prc)
		{
			ASSERT(m_spvis);
			CopyRect(prc, &m_rectangle);
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetViewInset(LPRECT prc)
		{
			prc->left = prc->right = prc->top = prc->bottom = 0;
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetCharFormat(const CHARFORMATW **ppCF)
		{
			*ppCF = &m_cf;
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetParaFormat(const PARAFORMAT **ppPF)
		{
			*ppPF = &m_pf;
			return S_OK;
		}

		COLORREF TinyTextHost::TxGetSysColor(int nIndex)
		{
			return GetSysColor(nIndex);
		}

		HRESULT TinyTextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
		{
			*pstyle = TXTBACK_TRANSPARENT;
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetMaxLength(DWORD *plength)
		{
			*plength = m_limit == -1 ? INFINITE : m_limit;
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
		{
			*pdwScrollBar = m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL);
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetPasswordChar(_Out_ TCHAR *pch)
		{
			*pch = m_password;
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetAcceleratorPos(LONG *pcp)
		{
			return S_OK;
		}

		HRESULT TinyTextHost::TxGetExtent(LPSIZEL lpExtent)
		{
			*lpExtent = m_extent;
			return S_OK;
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
			DWORD bits = TXTBIT_RICHTEXT | TXTBIT_WORDWRAP | TXTBIT_SCROLLBARCHANGE | TXTBIT_CHARFORMATCHANGE | TXTBIT_PARAFORMATCHANGE | TXTBIT_EXTENTCHANGE | TXTBIT_CLIENTRECTCHANGE;
			if (m_dwStyle & ES_MULTILINE)
			{
				bits |= TXTBIT_MULTILINE;
			}
			if (m_dwStyle & ES_READONLY)
			{
				bits |= TXTBIT_READONLY;
			}
			if (m_dwStyle & ES_PASSWORD)
			{
				bits |= TXTBIT_USEPASSWORD;
			}
			if (!(m_dwStyle & ES_NOHIDESEL))
			{
				bits |= TXTBIT_HIDESELECTION;
			}
			*pdwBits = bits & dwMask;
			return S_OK;
		}

		HRESULT TinyTextHost::TxNotify(DWORD iNotify, void *pv)
		{
			return S_OK;
		}

		HIMC TinyTextHost::TxImmGetContext()
		{
			ASSERT(m_spvis);
			return ImmGetContext(m_spvis->m_document->Handle());
		}

		void TinyTextHost::TxImmReleaseContext(HIMC himc)
		{
			ASSERT(m_spvis);
			ImmReleaseContext(m_spvis->m_document->Handle(), himc);
		}

		HRESULT TinyTextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth)
		{
			*lSelBarWidth = 0;
			return S_OK;
		}
	}
}

