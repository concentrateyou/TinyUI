#include "../stdafx.h"
#include "TinyDialog.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyDialogBase, TinyHandleHWND);
	TinyDialogBase::TinyDialogBase() :
		m_bModal(FALSE),
		m_iDlgResult(0),
		m_hPrimaryProc(NULL),
		m_pTemplateName(NULL)
	{

	}
	TinyDialogBase::~TinyDialogBase()
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
	void TinyDialogBase::PreSubclassDialog()
	{

	}
	BOOL TinyDialogBase::PreTranslateMessage(MSG* pMsg)
	{
		return ::IsDialogMessage(m_hWND, pMsg);
	}
	LRESULT TinyDialogBase::DefWindowProc()
	{
		return FALSE;
	}
	DWORD TinyDialogBase::GetDefID() const
	{
		ASSERT(::IsWindow(m_hWND));
		return DWORD(::SendMessage(m_hWND, DM_GETDEFID, 0, 0));
	}
	void TinyDialogBase::SetDefID(UINT nID)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, DM_SETDEFID, nID, 0);
	}
	void TinyDialogBase::NextDlgCtrl() const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, 0, 0);
	}
	void TinyDialogBase::PrevDlgCtrl() const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, 1, 0);
	}
	void TinyDialogBase::GotoDlgCtrl(HWND hWND)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_NEXTDLGCTL, (WPARAM)hWND, 1L);
	}
	INT_PTR CALLBACK TinyDialogBase::BeginLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyDialogBase *_this = NULL;
		if (uMsg == WM_INITDIALOG)
		{
			ASSERT(lParam != NULL);
			_this = (TinyDialogBase*)lParam;
			if (_this != NULL)
			{
				if (_this->IsModal())
				{
					_this->Attach(hWND);
				}
				_this->m_thunk.Initialize((WNDPROC)TinyDialogBase::EndLoop, _this);
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
	INT_PTR CALLBACK TinyDialogBase::EndLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyDialogBase* _this = (TinyDialogBase*)hWND;
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
	BOOL TinyDialogBase::Create(HWND hParent, LPCTSTR lpTemplateName)
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
		HWND hWND = ::CreateDialogParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, TinyDialogBase::BeginLoop, (LPARAM)this);
		return Attach(hWND);
	}
	BOOL TinyDialogBase::Create(HWND hParent, WORD wInteger)
	{
		this->m_wInteger = wInteger;
		this->m_pTemplateName = MAKEINTRESOURCE(wInteger);
		return Create(hParent, this->m_pTemplateName);
	}
	BOOL TinyDialogBase::SubclassDialog(HWND hWND)
	{
		PreSubclassDialog();
		ASSERT(::IsWindow(hWND));
		BOOL result = m_thunk.Initialize((WNDPROC)TinyDialogBase::EndLoop, this);
		if (result == FALSE) return FALSE;
		DLGPROC hProc = (DLGPROC)m_thunk.GetWNDPROC();
		DLGPROC hDlgProc = (DLGPROC)::SetWindowLongPtr(hWND, DWLP_DLGPROC, (LONG_PTR)hProc);
		if (hDlgProc == NULL)
			return FALSE;
		m_hPrimaryProc = hDlgProc;
		m_hWND = hWND;
		return TRUE;
	}
	INT_PTR TinyDialogBase::DoModal(HWND hParent, WORD wInteger)
	{
		this->m_wInteger = wInteger;
		this->m_pTemplateName = MAKEINTRESOURCE(wInteger);
		return DoModal(hParent, this->m_pTemplateName);
	}
	INT_PTR TinyDialogBase::DoModal(HWND hParent, LPCTSTR lpTemplateName)
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
		m_iDlgResult = ::DialogBoxParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, BeginLoop, (LPARAM)this);
		return m_iDlgResult;
	}
	BOOL TinyDialogBase::MapDialogRect(LPRECT lpRect)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::MapDialogRect(m_hWND, lpRect);
	}
	BOOL TinyDialogBase::EndDialog()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::EndDialog(Detach(), m_iDlgResult);
	}
	BOOL TinyDialogBase::EndDialog(INT_PTR m_DlgResult)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::EndDialog(Detach(), m_DlgResult);
	}
	BOOL TinyDialogBase::EndDialog(HWND hWND, INT_PTR m_DlgResult)
	{
		ASSERT(::IsWindow(hWND));
		return ::EndDialog(Detach(), m_DlgResult);
	}
	BOOL TinyDialogBase::IsModal() const throw()
	{
		return m_bModal;
	}
	void TinyDialogBase::OnFinalMessage(HWND hWND)
	{

	}
	BOOL TinyDialogBase::DestroyWindow()
	{
		return ::DestroyWindow(Detach());
	}

	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyDialog, TinyDialogBase);

	TinyDialog::TinyDialog()
		: m_pMenu(NULL)
	{

	}

	TinyDialog::~TinyDialog()
	{

	}

	LRESULT TinyDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_CREATE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_DESTORY(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_SETCURSOR(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (lParam != NULL)
		{
			return ::SendMessage((HWND)lParam, (WM_COMMANDREFLECT), wParam, lParam);
		}
		else
		{
			bHandled = FALSE;
			//²Ëµ¥ClickÊÂ¼þ
			INT wID = LOWORD(wParam);
			if (m_pMenu != NULL)
			{
				m_pMenu->OnClick(m_pMenu, wID);
			}
			return TRUE;
		}
		return FALSE;
	}
	LRESULT TinyDialog::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_SIZE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyDialog::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_MOUSEMOVE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_MOUSELEAVE(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LBUTTONDOWN(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LBUTTONUP(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_LDBCLICK(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_RBUTTONDOWN(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_RBUTTONUP(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		EVENT_RDBCLICK(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyDialog::OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyDialog::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;;
		return FALSE;
	}
	LRESULT TinyDialog::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPNMHDR p = (LPNMHDR)lParam;
		return ::SendMessage(p->hwndFrom, (uMsg + WM_REFLECT), wParam, lParam);
	}
	LRESULT TinyDialog::OnCommandReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnNotifyReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnDeleteItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnDrawItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnMeasureItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnCut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnClear(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT TinyDialog::OnIniMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyHandleHMENU** pps = TinyApplication::GetInstance()->GetMapHMENU().Lookup(reinterpret_cast<HMENU>(wParam));
		if (pps != NULL)
		{
			m_pMenu = static_cast<TinyMenu*>(*pps);
		}
		return FALSE;
	}
	LRESULT TinyDialog::OnIniMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyHandleHMENU** pps = TinyApplication::GetInstance()->GetMapHMENU().Lookup(reinterpret_cast<HMENU>(wParam));
		if (pps != NULL)
		{
			m_pMenu = static_cast<TinyMenu*>(*pps);
		}
		return FALSE;
	}
	LRESULT TinyDialog::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_POSCHANGING(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		EVENT_POSCHANGED(uMsg, wParam, lParam, bHandled);
		return FALSE;
	}
	LRESULT TinyDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	BOOL TinyDialog::ShowWindow(INT nCmdShow) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ShowWindow(m_hWND, nCmdShow);
	}

	BOOL TinyDialog::UpdateWindow() throw()
	{
		ASSERT(m_hWND != NULL);
		return ::UpdateWindow(m_hWND);
	}
	BOOL TinyDialog::Invalidate()
	{
		return InvalidateRect(m_hWND, NULL, TRUE);
	}
	void TinyDialog::CenterWindow(HWND parent, SIZE size) throw()
	{
		RECT windowBounds;
		RECT centerBounds = { 0 };
		if (parent)
		{
			::GetWindowRect(parent, &centerBounds);
		}
		else
		{
			HMONITOR monitor = MonitorFromWindow(m_hWND, MONITOR_DEFAULTTONEAREST);
			if (monitor != NULL)
			{
				MONITORINFO mi = { 0 };
				mi.cbSize = sizeof(mi);
				GetMonitorInfo(monitor, &mi);
				centerBounds = mi.rcWork;
			}
		}
		windowBounds.left = centerBounds.left + (centerBounds.right - centerBounds.left - size.cx) / 2;
		windowBounds.right = windowBounds.left + size.cx;
		windowBounds.top = centerBounds.top + (centerBounds.bottom - centerBounds.top - size.cy) / 2;
		windowBounds.bottom = windowBounds.top + size.cy;
		if (::GetWindowLong(m_hWND, GWL_STYLE) & WS_CHILD)
		{
			POINT topleft = { windowBounds.left, windowBounds.top };
			::MapWindowPoints(HWND_DESKTOP, parent, &topleft, 1);
			windowBounds.left = topleft.x;
			windowBounds.top = topleft.y;
			windowBounds.right = windowBounds.left + size.cx;
			windowBounds.bottom = windowBounds.top + size.cy;
		}
		WINDOWINFO win_info = { 0 };
		win_info.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(m_hWND, &win_info);
		if (AdjustWindowRectEx(&windowBounds, win_info.dwStyle, FALSE, win_info.dwExStyle))
		{
			SetWindowPos(m_hWND, 0, windowBounds.left, windowBounds.top,
				windowBounds.right - windowBounds.left,
				windowBounds.bottom - windowBounds.top,
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
	DWORD TinyDialog::GetStyle() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)GetWindowLong(m_hWND, GWL_STYLE);
	}
	DWORD TinyDialog::GetExStyle() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)GetWindowLong(m_hWND, GWL_EXSTYLE);
	}
	BOOL  TinyDialog::GetWindowRect(LPRECT lprect)
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)::GetWindowRect(m_hWND, lprect);
	}
	BOOL  TinyDialog::GetClientRect(LPRECT lprect)
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD)::GetClientRect(m_hWND, lprect);
	}

	BOOL TinyDialog::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		DWORD dwStyle = ::GetWindowLong(m_hWND, GWL_STYLE);
		DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
		if (dwStyle == dwNewStyle)
			return FALSE;
		::SetWindowLong(m_hWND, GWL_STYLE, dwNewStyle);
		if (nFlags != 0)
		{
			return ::SetWindowPos(m_hWND, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
		}
		return TRUE;
	}

	BOOL TinyDialog::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		DWORD dwStyle = ::GetWindowLong(m_hWND, GWL_EXSTYLE);
		DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
		if (dwStyle == dwNewStyle)
			return FALSE;
		::SetWindowLong(m_hWND, GWL_EXSTYLE, dwNewStyle);
		if (nFlags != 0)
		{
			return ::SetWindowPos(m_hWND, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
		}
		return TRUE;
	}
	BOOL TinyDialog::SetSize(INT cx, INT cy) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyDialog::SetPosition(INT x, INT y) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyDialog::SetSize(const TinySize& size) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyDialog::SetPosition(const TinyPoint& pos) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowPos(m_hWND, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	BOOL TinyDialog::ClientToScreen(LPPOINT lpPoint) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ClientToScreen(m_hWND, lpPoint);
	}

	BOOL TinyDialog::ClientToScreen(LPRECT lpRect) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		if (!::ClientToScreen(m_hWND, (LPPOINT)lpRect))
			return FALSE;
		return ::ClientToScreen(m_hWND, ((LPPOINT)lpRect) + 1);
	}

	BOOL TinyDialog::ScreenToClient(LPPOINT lpPoint) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ScreenToClient(m_hWND, lpPoint);
	}

	BOOL TinyDialog::ScreenToClient(LPRECT lpRect) const throw()
	{
		ASSERT(::IsWindow(m_hWND));
		if (!::ScreenToClient(m_hWND, (LPPOINT)lpRect))
			return FALSE;
		return ::ScreenToClient(m_hWND, ((LPPOINT)lpRect) + 1);
	}
	void TinyDialog::SetDefaultFont()
	{
		::SendMessage(m_hWND, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
	}
	void TinyDialog::SetFont(HFONT hFont, BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, WM_SETFONT, (WPARAM)hFont, bRedraw);
	}
	HFONT TinyDialog::GetFont() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HFONT)::SendMessage(m_hWND, WM_GETFONT, 0, 0);
	}
}

