#include "../stdafx.h"
#include "TinyTextHost.h"

namespace TinyUI
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

	TinyTextHost::TinyTextHost(TinyRichText* pRichText)
		:m_hInstance(NULL),
		m_pRichText(pRichText)
	{

	}
	TinyTextHost::~TinyTextHost()
	{
		if (m_hInstance)
		{
			::FreeLibrary(m_hInstance);
			m_hInstance = NULL;
		}
	}
	BOOL TinyTextHost::Initialize()
	{
#if (_RICHEDIT_VER >= 0x0200)
		m_hInstance = ::LoadLibrary(TEXT("msftedit.dll"));
#elif (_RICHEDIT_VER > 0x100)
		m_hInstance = ::LoadLibrary(TEXT("RICHED20.dll"));
#else
		m_hInstance = ::LoadLibrary(TEXT("RICHED32.dll"));
#endif
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
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	HDC TinyTextHost::TxGetDC()
	{
		return ::GetDC(NULL);
	}

	INT TinyTextHost::TxReleaseDC(HDC hdc)
	{
		return ::ReleaseDC(NULL, hdc);
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
		return FALSE;
	}

	BOOL TinyTextHost::TxShowCaret(BOOL fShow)
	{
		return FALSE;
	}

	BOOL TinyTextHost::TxSetCaretPos(INT x, INT y)
	{
		return FALSE;
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
		return FALSE;
	}

	BOOL TinyTextHost::TxClientToScreen(LPPOINT lppt)
	{
		return FALSE;
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
		return FALSE;
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

	COLORREF TinyTextHost::TxGetSysColor(int nIndex)
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

	HRESULT TinyTextHost::TxGetPasswordChar(_Out_ TCHAR *pch)
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
		return FALSE;
	}

	void TinyTextHost::TxImmReleaseContext(HIMC himc)
	{

	}

	HRESULT TinyTextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth)
	{
		return FALSE;
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

}

