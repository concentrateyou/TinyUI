#include "../stdafx.h"
#include "TinyDialog.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyDialog, TinyHandleHWND);

	TinyDialog::TinyDialog() :
		m_bModal(FALSE),
		m_iDlgResult(0),
		m_hPrimaryProc(reinterpret_cast<DLGPROC>(::DefWindowProc))
	{

	}
	TinyDialog::~TinyDialog()
	{
		if (m_hWND != NULL)
		{
			if (IsModal())
			{
				EndDialog(m_iDlgResult);
			}
			else
			{
				DestroyWindow();
			}
			m_iDlgResult = 0;
			m_bModal = FALSE;
		}
	}
	void TinyDialog::PreSubclassDialog()
	{

	}
	BOOL TinyDialog::PreTranslateMessage(MSG* pMsg)
	{
		return ::IsDialogMessage(m_hWND, pMsg);
	}
	LRESULT TinyDialog::DefWindowProc()
	{
		return FALSE;
	}
	DWORD TinyDialog::GetDefID() const
	{
		ASSERT(::IsWindow(m_hWND));
		return DWORD(::SendMessage(m_hWND, DM_GETDEFID, 0, 0));
	}
	void TinyDialog::SetDefID(UINT nID)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, DM_SETDEFID, nID, 0);
	}
	void TinyDialog::NextDlgCtrl() const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, 0, 0);
	}
	void TinyDialog::PrevDlgCtrl() const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, 1, 0);
	}
	void TinyDialog::GotoDlgCtrl(HWND hWND)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, (WPARAM)hWND, 1L);
	}
	INT_PTR CALLBACK TinyDialog::BeginLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyDialog *_this = NULL;
		if (uMsg == WM_INITDIALOG)
		{
			ASSERT(lParam != NULL);
			_this = (TinyDialog*)lParam;
			if (_this != NULL)
			{
				if (_this->IsModal())
				{
					_this->Attach(hWND);
				}
				_this->m_thunk.Initialize((WNDPROC)TinyDialog::EndLoop, _this);
				DLGPROC hProc = (DLGPROC)_this->m_thunk.GetWNDPROC();
				DLGPROC hOldProc = (DLGPROC)::SetWindowLongPtr(hWND, DWLP_DLGPROC, (LONG_PTR)hProc);
				if (hOldProc != BeginLoop)
					TRACE(_T("Subclassing through a hook discarded.\n"));
				DBG_UNREFERENCED_LOCAL_VARIABLE(hOldProc);
				return hProc(hWND, uMsg, wParam, lParam);
			}
		}
		return FALSE;
	};
	INT_PTR CALLBACK TinyDialog::EndLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyDialog* _this = (TinyDialog*)hWND;
		TinyMsg msg(_this->m_hWND, uMsg, wParam, lParam);
		const TinyMsg* pOldMsg = _this->m_pCurrentMsg;
		_this->m_pCurrentMsg = &msg;
		LRESULT lRes;
		BOOL bRet = _this->ProcessWindowMessage(_this->m_hWND, uMsg, wParam, lParam, lRes);
		ASSERT(_this->m_pCurrentMsg == &msg);
		_this->m_pCurrentMsg = pOldMsg;
		//ms-help://MS.VSCC.v90/MS.MSDNQTR.v90.chs/winui/winui/windowsuserinterface/windowing/dialogboxes/dialogboxreference/dialogboxfunctions/dialogproc.htm
		//IF bRet==1 process the message
		if (bRet)
		{
			switch (uMsg)
			{
			case WM_COMPAREITEM:
			case WM_VKEYTOITEM:
			case WM_CHARTOITEM:
			case WM_INITDIALOG:
			case WM_QUERYDRAGICON:
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
				bRet = (BOOL)lRes;
				break;
			default:
				if ((_this->m_dwState & 0x00000001) == 0)
				{
					::SetWindowLongPtr(_this->m_hWND, DWLP_MSGRESULT, lRes);
				}
				break;
			}
		}
		else if (uMsg == WM_NCDESTROY)
		{
			_this->m_dwState |= 0x00000001;
		}
		if ((_this->m_dwState & 0x00000001) && _this->m_pCurrentMsg == NULL)
		{
			HWND hWNDThis = _this->m_hWND;
			_this->m_hWND = NULL;
			_this->m_dwState &= ~0x00000001;
			_this->OnFinalMessage(hWNDThis);
		}
		return (INT_PTR)bRet;
	}
	BOOL TinyDialog::Create(HWND hParent, LPCTSTR lpTemplateName)
	{
		ASSUME(m_hWND == NULL);
		if (!m_thunk.Initialize(NULL, NULL))
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return FALSE;
		}
		m_bModal = FALSE;
		HWND hWND = ::CreateDialogParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, TinyDialog::BeginLoop, (LPARAM)this);
		return Attach(hWND);
	}
	BOOL TinyDialog::Create(HWND hParent, WORD wInteger)
	{
		return Create(hParent, MAKEINTRESOURCE(wInteger));
	}
	BOOL TinyDialog::SubclassDialog(HWND hWND)
	{
		ASSERT(::IsWindow(hWND));
		PreSubclassDialog();
		if (!m_thunk.Initialize((WNDPROC)TinyDialog::EndLoop, this))
			return FALSE;
		DLGPROC hProc = (DLGPROC)m_thunk.GetWNDPROC();
		DLGPROC hDlgProc = (DLGPROC)::SetWindowLongPtr(hWND, DWLP_DLGPROC, (LONG_PTR)hProc);
		if (hDlgProc == NULL)
			return FALSE;
		m_hPrimaryProc = hDlgProc;
		m_hWND = hWND;
		return TRUE;
	}
	HWND TinyDialog::UnsubclassDialog()
	{
		ASSERT(::IsWindow(m_hWND));
		DLGPROC hProc = (DLGPROC)m_thunk.GetWNDPROC();
		DLGPROC hDlgProc = (DLGPROC)::GetWindowLongPtr(m_hWND, GWLP_WNDPROC);
		HWND hWND = NULL;
		if (hProc == hDlgProc)
		{
			if (!::SetWindowLongPtr(m_hWND, GWLP_WNDPROC, (LONG_PTR)m_hPrimaryProc))
				return NULL;
			m_hPrimaryProc = reinterpret_cast<DLGPROC>(::DefWindowProc);
			hWND = Detach();
		}
		return hWND;
	}
	INT_PTR TinyDialog::DoModal(HWND hParent, WORD wInteger)
	{
		return DoModal(hParent, MAKEINTRESOURCE(wInteger));
	}
	INT_PTR TinyDialog::DoModal(HWND hParent, LPCTSTR lpTemplateName)
	{
		ASSUME(m_hWND == NULL);
		if (!m_thunk.Initialize(NULL, NULL))
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return -1;
		}
		m_bModal = TRUE;
		m_iDlgResult = ::DialogBoxParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, BeginLoop, (LPARAM)this);
		return m_iDlgResult;
	}
	BOOL TinyDialog::MapDialogRect(LPRECT lpRect)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::MapDialogRect(m_hWND, lpRect);
	}
	BOOL TinyDialog::EndDialog()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::EndDialog(Detach(), m_iDlgResult);
	}
	BOOL TinyDialog::EndDialog(INT_PTR m_DlgResult)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::EndDialog(Detach(), m_DlgResult);
	}
	BOOL TinyDialog::EndDialog(HWND hWND, INT_PTR m_DlgResult)
	{
		ASSERT(::IsWindow(hWND));
		return ::EndDialog(Detach(), m_DlgResult);
	}
	BOOL TinyDialog::IsModal() const throw()
	{
		return m_bModal;
	}
	void TinyDialog::OnFinalMessage(HWND hWND)
	{

	}
	BOOL TinyDialog::DestroyWindow()
	{
		return ::DestroyWindow(Detach());
	}
}

