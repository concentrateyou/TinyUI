#include "../stdafx.h"
#include "TinyDialog.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyDialog, TinyHandleHWND);
	TinyDialog::TinyDialog() :
		m_bModal(FALSE),
		m_iDlgResult(0),
		m_hPrimaryProc(NULL),
		m_pTemplateName(NULL)
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
	/*LRESULT CALLBACK TinyDialog::GetMsgProc(INT code, WPARAM wParam, LPARAM lParam)
	{
		LPMSG pMsg = (LPMSG)lParam;
		TinyHandleHWND** pHWND = TinyApplication::GetInstance()->GetMapHWND().Lookup(pMsg->hwnd);
		if (pHWND != NULL)
		{

		}
	}*/
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
		this->m_pTemplateName = lpTemplateName;
		BOOL result;
		ASSUME(m_hWND == NULL);
		result = m_thunk.Initialize(NULL, NULL);
		if (result == FALSE)
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return NULL;
		}
		m_bModal = FALSE;
		HWND hWND = ::CreateDialogParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, TinyDialog::BeginLoop, (LPARAM)this);
		return Attach(hWND);
	}
	BOOL TinyDialog::Create(HWND hParent, WORD wInteger)
	{
		this->m_wInteger = wInteger;
		this->m_pTemplateName = MAKEINTRESOURCE(wInteger);
		return Create(hParent, this->m_pTemplateName);
	}
	BOOL TinyDialog::SubclassDialog(HWND hWND)
	{
		PreSubclassDialog();
		ASSERT(::IsWindow(hWND));
		BOOL result = m_thunk.Initialize((WNDPROC)TinyDialog::EndLoop, this);
		if (result == FALSE) return FALSE;
		DLGPROC hProc = (DLGPROC)m_thunk.GetWNDPROC();
		DLGPROC hDlgProc = (DLGPROC)::SetWindowLongPtr(hWND, DWLP_DLGPROC, (LONG_PTR)hProc);
		if (hDlgProc == NULL)
			return FALSE;
		m_hPrimaryProc = hDlgProc;
		m_hWND = hWND;
		return TRUE;
	}
	INT_PTR TinyDialog::DoModal(HWND hParent, WORD wInteger)
	{
		this->m_wInteger = wInteger;
		this->m_pTemplateName = MAKEINTRESOURCE(wInteger);
		return DoModal(hParent, this->m_pTemplateName);
	}
	INT_PTR TinyDialog::DoModal(HWND hParent, LPCTSTR lpTemplateName)
	{
		this->m_pTemplateName = MAKEINTRESOURCE(m_wInteger);
		BOOL result;
		ASSUME(m_hWND == NULL);
		result = m_thunk.Initialize(NULL, NULL);
		if (result == FALSE)
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return -1;
		}
		m_bModal = TRUE;
		//m_hMsgHook = ::SetWindowsHookEx(WH_GETMESSAGE, TinyDialog::GetMsgProc, TinyApplication::GetInstance()->Handle(), GetCurrentThreadId());
		//ASSERT(m_hMsgHook != NULL);
		m_iDlgResult = ::DialogBoxParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, BeginLoop, (LPARAM)this);
		return m_iDlgResult;
	}
	//HRESULT TinyDialog::ModalLoop(DWORD dwFlags)
	//{
	//	BOOL bIdle = TRUE;
	//	LONG lIdleCount = 0;
	//	BOOL bShowIdle = (dwFlags & MLF_SHOWONIDLE) && !(GetStyle() & WS_VISIBLE);
	//	HWND hParent = ::GetParent(m_hWND);
	//	MSG msg;
	//	for (;;)
	//	{
	//		ASSERT((m_nFlags & WF_CONTINUEMODAL) != 0);
	//		while (bIdle && !::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	//		{
	//			ASSERT((m_nFlags & WF_CONTINUEMODAL) != 0);
	//			if (bShowIdle)
	//			{
	//				ShowWindow(SW_SHOWNORMAL);
	//				UpdateWindow();
	//				bShowIdle = FALSE;
	//			}
	//			if (!(dwFlags & MLF_NOIDLEMSG) && hParent != NULL && lIdleCount == 0)
	//			{
	//				::SendMessage(hParent, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWND);
	//			}
	//			if ((dwFlags & MLF_NOKICKIDLE) || !SendMessage(WM_KICKIDLE, MSGF_DIALOGBOX, lIdleCount++))
	//			{
	//				bIdle = FALSE;
	//			}
	//		}
	//		do
	//		{
	//			ASSERT((m_nFlags & WF_CONTINUEMODAL) != 0);
	//			if (!::GetMessage(&msg, NULL, NULL, NULL))
	//			{
	//				if (msg.message != WM_KICKIDLE && !PreTranslateMessage(&msg))
	//				{
	//					::TranslateMessage(&msg);
	//					::DispatchMessage(&msg);
	//				}
	//				return FALSE;
	//			}
	//			else
	//			{
	//				::PostQuitMessage(0);
	//				return -1;
	//			}
	//			if (bShowIdle && (msg->message == 0x118 || msg->message == WM_SYSKEYDOWN))
	//			{
	//				ShowWindow(SW_SHOWNORMAL);
	//				UpdateWindow();
	//				bShowIdle = FALSE;
	//			}
	//			if ((m_nFlags & WF_CONTINUEMODAL) == 0)
	//				goto END;
	//			if (AfxIsIdleMessage(pMsg))
	//			{
	//				bIdle = TRUE;
	//				lIdleCount = 0;
	//			}
	//		} while (::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE));
	//	}
	//END:
	//	m_nFlags &= ~(WF_MODALLOOP | WF_CONTINUEMODAL);
	//	return m_nModalResult;
	//}
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

