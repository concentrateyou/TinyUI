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
			m_chPwd('*'),
			m_hDll(NULL)
		{
			m_hDll = ::LoadLibrary(TEXT("Msftedit.dll"));
			if (m_hDll != NULL)
			{
				PCreateTextServices ps = (PCreateTextServices)GetProcAddress(m_hDll, "CreateTextServices");
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
			ZeroMemory(&m_pf, sizeof(PARAFORMAT));
			m_pf.cbSize = sizeof(PARAFORMAT);
			ZeroMemory(&m_cf, sizeof(CHARFORMATW));
			m_cf.cbSize = sizeof(CHARFORMATW);
		}
		TinyTextHost::~TinyTextHost()
		{
			if (m_pts)
			{
				m_pts->OnTxInPlaceDeactivate();
				m_pts->Release();
			}
			if (m_hDll != NULL)
			{
				::FreeLibrary(m_hDll);
				m_hDll = NULL;
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
			*prc = s;
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetViewInset(LPRECT prc)
		{
			memset(prc, 0, sizeof(RECT));
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
			*plength = m_dwLimit;
			return S_OK;
		}
		HRESULT TinyTextHost::TxGetScrollBars(DWORD *pdwScrollBar)
		{
			return FALSE;
		}
		HRESULT TinyTextHost::TxGetPasswordChar(TCHAR *pch)
		{
			*pch = m_chPwd;
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
			ASSERT(m_spvis);
			/*	DWORD dwProperties = 0;
				if (fRich)
				{
				dwProperties = TXTBIT_RICHTEXT;
				}
				if (m_dwStyle & ES_MULTILINE)
				{
				dwProperties |= TXTBIT_MULTILINE;
				}
				if (m_dwStyle & ES_READONLY)
				{
				dwProperties |= TXTBIT_READONLY;
				}
				if (m_dwStyle & ES_PASSWORD)
				{
				dwProperties |= TXTBIT_USEPASSWORD;
				}
				if (!(m_dwStyle & ES_NOHIDESEL))
				{
				dwProperties |= TXTBIT_HIDESELECTION;
				}
				if (fEnableAutoWordSel)
				{
				dwProperties |= TXTBIT_AUTOWORDSEL;
				}
				if (fVertical)
				{
				dwProperties |= TXTBIT_VERTICAL;
				}

				if (fWordWrap)
				{
				dwProperties |= TXTBIT_WORDWRAP;
				}

				if (fAllowBeep)
				{
				dwProperties |= TXTBIT_ALLOWBEEP;
				}

				if (fSaveSelection)
				{
				dwProperties |= TXTBIT_SAVESELECTION;
				}

				*pdwBits = dwProperties & dwMask;*/
			return NOERROR;
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
		BOOL TinyTextHost::SetCharFormat(HFONT hFont, COLORREF crTextColor, LONG yOffset)
		{
			ASSERT(m_pts);
			HWND hWND = m_spvis->m_document->GetVisualHWND()->Handle();
			LOGFONT lf;
			if (!::GetObject(hFont, sizeof(LOGFONT), &lf))
				return FALSE;
			m_cf.cbSize = sizeof(CHARFORMAT2);
			if (HDC hDC = ::GetDC(hWND))
			{
				m_cf.yHeight = lf.lfHeight * LY_PER_INCH / GetDeviceCaps(hDC, LOGPIXELSY);
				::ReleaseDC(hWND, hDC);
			}
			m_cf.yOffset = yOffset;
			m_cf.crTextColor = GetSysColor(COLOR_WINDOWTEXT);
			m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR | CFE_ITALIC;
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
			_tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
			MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, m_cf.szFaceName, LF_FACESIZE);
#endif
			return m_pts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE) == S_OK;
		}
		BOOL TinyTextHost::SetParaFormat(WORD wAlignment, LONG	dxStartIndent, LONG	dxRightIndent, LONG	dxOffset, WORD	wNumbering)
		{
			ASSERT(m_pts);
			m_pf.cbSize = sizeof(PARAFORMAT);
			m_pf.dwMask = PFM_ALL;
			m_pf.wAlignment = wAlignment;
			m_pf.dxStartIndent = dxStartIndent;
			m_pf.dxRightIndent = dxRightIndent;
			m_pf.dxOffset = dxOffset;
			m_pf.cTabCount = 1;
			m_pf.wNumbering = wNumbering;
			m_pf.rgxTabs[0] = lDefaultTab;
			return m_pts->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE) == S_OK;
		}
		BOOL TinyTextHost::SetRectangle(const TinyRectangle& rectangle)
		{
			ASSERT(m_pts);
			m_rectangle = rectangle;
			if (m_pts->OnTxInPlaceActivate(&rectangle) == S_OK)
			{
				LRESULT lRes = S_OK;
				m_pts->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lRes);
				return m_pts->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE) == S_OK;
			}
			return FALSE;
		}
		BOOL TinyTextHost::SetText(const string& str)
		{
			ASSERT(m_pts);
			wstring wstr = StringToWString(str);
			return m_pts->TxSetText(wstr.c_str()) != S_OK;
		}
		BOOL TinyTextHost::SetFocus(BOOL bFocus)
		{
			ASSERT(m_pts);
			return m_pts->TxSendMessage(WM_SETFOCUS, 0, 0, 0) == S_OK;
		}
	}
}
