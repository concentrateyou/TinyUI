#include "../stdafx.h"
#include "TinyTextHost.h"
#include "TinyVisual.h"
#include "TinyVisualDocument.h"
#pragma comment(lib,"imm32.lib")

namespace TinyUI
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
			:m_hInstance(NULL)
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
		BOOL TinyTextHost::Initialize(TinyVisual* spvis)
		{
			ASSERT(spvis);
			m_spvis = spvis;
			HDC hDC = GetDC(m_spvis->Handle());
			m_logpixelsx = ::GetDeviceCaps(hDC, LOGPIXELSX);
			m_logpixelsy = ::GetDeviceCaps(hDC, LOGPIXELSY);
			ReleaseDC(m_spvis->Handle(), hDC);
			LOGFONT lf;
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			GetObject(hFont, sizeof(LOGFONT), &lf);
			m_cf.cbSize = sizeof(CHARFORMAT2);
			m_cf.yHeight = lf.lfHeight * LY_PER_INCH / m_logpixelsy;
			m_cf.yOffset = 0;
			m_cf.crTextColor = GetSysColor(COLOR_WINDOWTEXT);
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

			memset(&m_pf, 0, sizeof(PARAFORMAT));
			m_pf.cbSize = sizeof(PARAFORMAT);
			m_pf.dwMask = PFM_ALL;
			m_pf.wAlignment = PFA_LEFT;
			m_pf.cTabCount = 1;
			m_pf.rgxTabs[0] = lDefaultTab;

			m_hInstance = ::LoadLibrary(TEXT("msftedit.dll"));
			if (m_hInstance)
			{
				pfnCreateTextServices ps = (pfnCreateTextServices)::GetProcAddress(m_hInstance, "CreateTextServices");
				if (ps != NULL)
				{
					IUnknown* pUnknown = NULL;
					if (ps(NULL, static_cast<ITextHost*>(this), &pUnknown) == S_OK)
					{
						pUnknown->QueryInterface(IID_ITextServices, (void**)&m_ts);
						pUnknown->Release();
						m_ts->OnTxInPlaceDeactivate();
						m_ts->OnTxUIDeactivate();
						m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
						m_ts->TxSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE, NULL);
						m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
						m_ts->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE);
						return TRUE;
					}
				}
			}
			return FALSE;
		}
		BOOL TinyTextHost::UpdateView()
		{
			ASSERT(m_ts || m_spvis);
			TinyRectangle rectangle = m_spvis->GetWindowRect();
			m_extent.cx = MAP_PIX_TO_LOGHIM(rectangle.Width(), m_logpixelsx);
			m_extent.cy = MAP_PIX_TO_LOGHIM(rectangle.Height(), m_logpixelsy);
			if (FAILED(m_ts->OnTxInPlaceActivate(&rectangle)))
				return FALSE;
			m_ts->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
			m_ts->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
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
			return ::GetDC(m_spvis->Handle());
		}

		INT TinyTextHost::TxReleaseDC(HDC hdc)
		{
			ASSERT(m_spvis);
			return ::ReleaseDC(m_spvis->Handle(), hdc);
		}

		BOOL TinyTextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
		{
			return S_OK;
		}

		BOOL TinyTextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
		{
			return S_OK;
		}

		BOOL TinyTextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
		{
			return S_OK;
		}

		BOOL TinyTextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
		{
			return S_OK;
		}

		void TinyTextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
		{
			ASSERT(m_spvis);
			::InvalidateRect(m_spvis->Handle(), prc, fMode);
		}

		void TinyTextHost::TxViewChange(BOOL fUpdate)
		{

		}

		BOOL TinyTextHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
		{
			ASSERT(m_spvis);
			return ::CreateCaret(m_spvis->Handle(), hbmp, xWidth, yHeight);
		}

		BOOL TinyTextHost::TxShowCaret(BOOL fShow)
		{
			ASSERT(m_spvis);
			if (fShow)
			{
				return ::ShowCaret(m_spvis->Handle());
			}
			else
			{
				return ::HideCaret(m_spvis->Handle());
			}
		}

		BOOL TinyTextHost::TxSetCaretPos(INT x, INT y)
		{
			return ::SetCaretPos(x, y);
		}

		BOOL TinyTextHost::TxSetTimer(UINT idTimer, UINT uTimeout)
		{
			return S_OK;
		}

		void TinyTextHost::TxKillTimer(UINT idTimer)
		{

		}

		void TinyTextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
		{

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
			return ::ScreenToClient(m_spvis->Handle(), lppt);
		}

		BOOL TinyTextHost::TxClientToScreen(LPPOINT lppt)
		{
			return ::ClientToScreen(m_spvis->Handle(), lppt);
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
			TinyRectangle rectangle = m_spvis->GetWindowRect();
			CopyRect(prc, &rectangle);
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
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
		{
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::TxGetPasswordChar(_Out_ TCHAR *pch)
		{
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::TxGetAcceleratorPos(LONG *pcp)
		{
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::TxGetExtent(LPSIZEL lpExtent)
		{
			*lpExtent = m_extent;
			return S_OK;
		}

		HRESULT TinyTextHost::OnTxCharFormatChange(const CHARFORMATW * pCF)
		{
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::OnTxParaFormatChange(const PARAFORMAT * pPF)
		{
			return E_NOTIMPL;
		}

		HRESULT TinyTextHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
		{
			DWORD bits = TXTBIT_MULTILINE | TXTBIT_RICHTEXT | TXTBIT_WORDWRAP;
			*pdwBits = bits & dwMask;
			return S_OK;
		}

		HRESULT TinyTextHost::TxNotify(DWORD iNotify, void *pv)
		{
			return E_NOTIMPL;
		}

		HIMC TinyTextHost::TxImmGetContext()
		{
			ASSERT(m_spvis);
			return ImmGetContext(m_spvis->Handle());
		}

		void TinyTextHost::TxImmReleaseContext(HIMC himc)
		{
			ASSERT(m_spvis);
			ImmReleaseContext(m_spvis->Handle(), himc);
		}

		HRESULT TinyTextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth)
		{
			*lSelBarWidth = 0;
			return S_OK;
		}
	}
}

