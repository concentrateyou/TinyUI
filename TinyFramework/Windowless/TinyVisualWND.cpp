#include "../stdafx.h"
#include "TinyVisualWND.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualHWND, TinyHandleHWND);

		TinyVisualHWND::TinyVisualHWND()
			:m_hPrimaryProc(::DefWindowProc),
			m_bModal(FALSE),
			m_bDialog(FALSE),
			m_iDlgResult(-1)
		{

		}
		TinyVisualHWND::~TinyVisualHWND()
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
		LPCSTR TinyVisualHWND::RetrieveClassName()
		{
			return NULL;
		}
		LPCSTR TinyVisualHWND::RetrieveTitle()
		{
			return NULL;
		}
		HICON TinyVisualHWND::RetrieveIcon()
		{
			return NULL;
		}
		DWORD TinyVisualHWND::RetrieveStyle()
		{
			return 0;
		}
		DWORD TinyVisualHWND::RetrieveExStyle()
		{
			return 0;
		}
		BOOL TinyVisualHWND::PreTranslateMessage(MSG* pMsg)
		{
			if (m_bDialog)
				return ::IsDialogMessage(m_hWND, pMsg);
			return FALSE;
		}
		void TinyVisualHWND::OnFinalMessage(HWND hWND)
		{

		}
		BOOL TinyVisualHWND::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
		{
			ASSUME(m_hWND == NULL);
			m_bModal = FALSE;
			m_bDialog = FALSE;
			if (!m_thunk.Initialize(NULL, NULL))
			{
				SetLastError(ERROR_OUTOFMEMORY);
				return FALSE;
			}
			HINSTANCE hInstance = TinyApplication::GetInstance()->Handle();
			if (hInstance == NULL)
			{
				SetLastError(ERROR_INVALID_DATA);
				return FALSE;
			}
			CREATESTRUCT cs;
			cs.lpszClass = RetrieveClassName();
			cs.lpszName = RetrieveTitle();
			cs.dwExStyle = RetrieveExStyle();
			cs.style = RetrieveStyle();
			cs.x = x;
			cs.y = y;
			cs.cx = cx;
			cs.cy = cy;
			cs.hwndParent = hParent;
			cs.hMenu = NULL;
			cs.hInstance = hInstance;
			cs.lpCreateParams = this;
			LPCTSTR lpszClass = RetrieveClass(cs.lpszClass);
			HWND hWND = ::CreateWindowEx(cs.dwExStyle, lpszClass, cs.lpszName,
				cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent,
				cs.hMenu, cs.hInstance, cs.lpCreateParams);
			DWORD a = GetLastError();
			return Attach(hWND);
		}
		BOOL TinyVisualHWND::Create(HWND hParent, WORD wInteger)
		{
			return Create(hParent, MAKEINTRESOURCE(wInteger));
		}
		BOOL TinyVisualHWND::Create(HWND hParent, LPCTSTR lpTemplateName)
		{
			ASSUME(m_hWND == NULL);
			m_bModal = FALSE;
			m_bDialog = TRUE;
			if (!m_thunk.Initialize(NULL, NULL))
			{
				SetLastError(ERROR_OUTOFMEMORY);
				return FALSE;
			}
			HWND hWND = ::CreateDialogParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, TinyVisualHWND::BeginLoopDLG, (LPARAM)this);
			return Attach(hWND);
		}
		BOOL TinyVisualHWND::DestroyWindow()
		{
			return ::DestroyWindow(Detach());
		}
		INT_PTR TinyVisualHWND::DoModal(HWND hParent, WORD wInteger)
		{
			return DoModal(hParent, MAKEINTRESOURCE(wInteger));
		}
		INT_PTR TinyVisualHWND::DoModal(HWND hParent, LPCTSTR lpTemplateName)
		{
			ASSUME(m_hWND == NULL);
			m_bModal = TRUE;
			m_bDialog = TRUE;
			if (!m_thunk.Initialize(NULL, NULL))
			{
				SetLastError(ERROR_OUTOFMEMORY);
				return -1;
			}
			m_iDlgResult = ::DialogBoxParam(TinyApplication::GetInstance()->Handle(), lpTemplateName, hParent, TinyVisualHWND::BeginLoopDLG, (LPARAM)this);
			return m_iDlgResult;
		}
		BOOL TinyVisualHWND::EndDialog()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::EndDialog(Detach(), m_iDlgResult);
		}
		BOOL TinyVisualHWND::EndDialog(INT_PTR m_DlgResult)
		{
			ASSERT(::IsWindow(m_hWND));
			return ::EndDialog(Detach(), m_DlgResult);
		}
		BOOL TinyVisualHWND::EndDialog(HWND hWND, INT_PTR m_DlgResult)
		{
			ASSERT(::IsWindow(hWND));
			return ::EndDialog(Detach(), m_DlgResult);
		}
		LPCSTR TinyVisualHWND::RetrieveClass(LPCTSTR lpszClass)
		{
			HINSTANCE hInstance = TinyApplication::GetInstance()->Handle();
			ASSERT(hInstance != NULL);
			ATOM atom = 0;
			WNDCLASSEX class_ex;
			memset(&class_ex, 0, sizeof(WNDCLASSEX));
			if (!::GetClassInfoEx(NULL, lpszClass, &class_ex))
			{
				if (!::GetClassInfoEx(hInstance, lpszClass, &class_ex))
				{
					class_ex.cbSize = sizeof(WNDCLASSEX);
					class_ex.lpfnWndProc = TinyVisualHWND::BeginLoopWND;
					class_ex.cbClsExtra = 0;
					class_ex.cbWndExtra = 0;
					class_ex.hInstance = hInstance;
					class_ex.hIcon = RetrieveIcon();
					class_ex.style &= ~CS_GLOBALCLASS;
					class_ex.style |= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
					class_ex.hCursor = LoadCursor(NULL, IDC_ARROW);
					class_ex.hbrBackground = GetSysColorBrush(0x0F);
					class_ex.lpszMenuName = NULL;
					class_ex.lpszClassName = lpszClass;
					class_ex.hIconSm = class_ex.hIcon;
					atom = ::RegisterClassEx(&class_ex);
					return MAKEINTATOM(atom);
				}
			}
			return lpszClass;
		};
		LRESULT TinyVisualHWND::BeginLoopWND(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			TinyVisualHWND *_this = NULL;
			if (uMsg == WM_NCCREATE)
			{
				ASSERT(lParam != NULL);
				_this = (TinyVisualHWND*)((LPCREATESTRUCT)lParam)->lpCreateParams;
				if (_this != NULL)
				{
					_this->m_hWND = hWND;
					_this->m_thunk.Initialize(TinyVisualHWND::EndLoopWND, _this);
					WNDPROC hProc = _this->m_thunk.GetWNDPROC();
					WNDPROC hOldProc = (WNDPROC)::SetWindowLongPtr(hWND, GWLP_WNDPROC, (LONG_PTR)hProc);
					if (hOldProc != BeginLoopWND)
					{
						ASSERT(_T("HOOK子类化失败.\n"));
					}
					return hProc(hWND, uMsg, wParam, lParam);
				}
			}
			return ::DefWindowProc(hWND, uMsg, wParam, lParam);
		};
		LRESULT TinyVisualHWND::EndLoopWND(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			TinyVisualHWND* _this = NULL;
			ASSERT(hWND);
			_this = (TinyVisualHWND*)hWND;
			if (_this != NULL)
			{
				TinyMsg msg(_this->m_hWND, uMsg, wParam, lParam);
				const TinyMsg* pOldMsg = _this->m_pCurrentMsg;
				_this->m_pCurrentMsg = &msg;
				LRESULT lRes = 0;
				BOOL bRet = _this->ProcessWindowMessage(_this->m_hWND, uMsg, wParam, lParam, lRes);
				ASSERT(_this->m_pCurrentMsg == &msg);
				if (!bRet)//如果没有设置Handle,调用CallWindowProc处理
				{
					if (uMsg != WM_NCDESTROY)
					{
						lRes = _this->DefWindowProc(uMsg, wParam, lParam);
					}
					else
					{
						//UnSubclass
						LONG_PTR hProc = ::GetWindowLongPtr(_this->m_hWND, GWLP_WNDPROC);
						lRes = _this->DefWindowProc(uMsg, wParam, lParam);
						WNDPROC hPrimaryProc = reinterpret_cast<WNDPROC>(_this->m_hPrimaryProc);
						if (hPrimaryProc != ::DefWindowProc && ::GetWindowLongPtr(_this->m_hWND, GWLP_WNDPROC) == hProc)
						{
							::SetWindowLongPtr(_this->m_hWND, GWLP_WNDPROC, (LONG_PTR)hPrimaryProc);
						}
						_this->m_dwState |= 0x00000001;//设置状态Destryed
					}
				}
				if ((_this->m_dwState & 0x00000001) && pOldMsg == NULL)
				{
					_this->m_dwState &= ~0x00000001;
					_this->m_pCurrentMsg = pOldMsg;
					_this->OnFinalMessage(_this->Detach());
				}
				else
				{
					_this->m_pCurrentMsg = pOldMsg;
				}
				return lRes;
			}
			return ::DefWindowProc(_this->m_hWND, uMsg, wParam, lParam);
		};
		INT_PTR CALLBACK TinyVisualHWND::BeginLoopDLG(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			TinyVisualHWND *_this = NULL;
			if (uMsg == WM_INITDIALOG)
			{
				ASSERT(lParam != NULL);
				_this = (TinyVisualHWND*)lParam;
				if (_this != NULL)
				{
					if (_this->IsModal())
					{
						_this->Attach(hWND);
					}
					_this->m_thunk.Initialize((WNDPROC)TinyVisualHWND::EndLoopDLG, _this);
					DLGPROC hProc = (DLGPROC)_this->m_thunk.GetWNDPROC();
					DLGPROC hOldProc = (DLGPROC)::SetWindowLongPtr(hWND, DWLP_DLGPROC, (LONG_PTR)hProc);
					if (hOldProc != BeginLoopDLG)
						TRACE(_T("Subclassing through a hook discarded.\n"));
					DBG_UNREFERENCED_LOCAL_VARIABLE(hOldProc);
					return hProc(hWND, uMsg, wParam, lParam);
				}
			}
			return FALSE;
		};
		INT_PTR CALLBACK TinyVisualHWND::EndLoopDLG(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			TinyVisualHWND* _this = (TinyVisualHWND*)hWND;
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
		LRESULT TinyVisualHWND::DefWindowProc()
		{
			LRESULT lRes = 0;
			const TinyMsg* pMsg = m_pCurrentMsg;
			if (pMsg != NULL)
				lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
			return lRes;
		}
		LRESULT TinyVisualHWND::DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
#ifdef STRICT
			return ::CallWindowProc(reinterpret_cast<WNDPROC>(m_hPrimaryProc), m_hWND, uMsg, wParam, lParam);
#else
			return ::CallWindowProc(reinterpret_cast<FARPROC>(m_hPrimaryProc), m_hWND, uMsg, wParam, lParam);
#endif
		}
		BOOL TinyVisualHWND::IsModal() const throw()
		{
			return m_bModal;
		}
		BOOL TinyVisualHWND::IsDialog() const throw()
		{
			return m_bDialog;
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNAMIC(TinyVisualWND, TinyVisualHWND);
		TinyVisualWND::TinyVisualWND()
		{

		}
		TinyVisualWND::~TinyVisualWND()
		{

		}
		LPCSTR	TinyVisualWND::RetrieveClassName()
		{
			return TEXT("TinyVisualFrame");
		}
		LPCSTR	TinyVisualWND::RetrieveTitle()
		{
			return TEXT("TinyVisualFrame");
		}
		HICON	TinyVisualWND::RetrieveIcon()
		{
			return NULL;
		}
		DWORD	TinyVisualWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
		DWORD	TinyVisualWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_OVERLAPPEDWINDOW);
		}
		LRESULT TinyVisualWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_CREATE(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_DESTORY(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_SETCURSOR(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (lParam != NULL)
			{
				return ::SendMessage((HWND)lParam, (WM_COMMANDREFLECT), wParam, lParam);
			}
			return FALSE;
		}
		LRESULT TinyVisualWND::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_SIZE(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_MOUSEMOVE(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_MOUSELEAVE(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_LBUTTONDOWN(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_LBUTTONUP(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_LDBCLICK(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_RBUTTONDOWN(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_RBUTTONUP(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;;
			EVENT_RDBCLICK(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			LPNMHDR p = (LPNMHDR)lParam;
			return ::SendMessage(p->hwndFrom, (uMsg + WM_REFLECT), wParam, lParam);
		}
		LRESULT TinyVisualWND::OnCommandReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnNotifyReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnDeleteItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnDrawItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnMeasureItemReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnCut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnClear(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnIniMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnIniMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_POSCHANGING(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			EVENT_POSCHANGED(uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		LRESULT TinyVisualWND::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualWND::OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		BOOL TinyVisualWND::ShowWindow(INT nCmdShow) throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::ShowWindow(m_hWND, nCmdShow);
		}

		BOOL TinyVisualWND::UpdateWindow() throw()
		{
			ASSERT(m_hWND != NULL);
			return ::UpdateWindow(m_hWND);
		}
		BOOL TinyVisualWND::Invalidate()
		{
			return InvalidateRect(m_hWND, NULL, TRUE);
		}
		void TinyVisualWND::CenterWindow(HWND parent, SIZE size) throw()
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
		DWORD TinyVisualWND::GetStyle() const
		{
			ASSERT(::IsWindow(m_hWND));
			return (DWORD)GetWindowLong(m_hWND, GWL_STYLE);
		}
		DWORD TinyVisualWND::GetExStyle() const
		{
			ASSERT(::IsWindow(m_hWND));
			return (DWORD)GetWindowLong(m_hWND, GWL_EXSTYLE);
		}
		BOOL  TinyVisualWND::GetWindowRect(LPRECT lprect)
		{
			ASSERT(::IsWindow(m_hWND));
			return (DWORD)::GetWindowRect(m_hWND, lprect);
		}
		BOOL  TinyVisualWND::GetClientRect(LPRECT lprect)
		{
			ASSERT(::IsWindow(m_hWND));
			return (DWORD)::GetClientRect(m_hWND, lprect);
		}

		BOOL TinyVisualWND::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
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

		BOOL TinyVisualWND::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags) throw()
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
		BOOL TinyVisualWND::SetShanow(BOOL bFlag)
		{
			ASSERT(::IsWindow(m_hWND));
			DWORD dwStyle = ::GetClassLong(m_hWND, GCL_STYLE);
			if (bFlag)
				dwStyle |= CS_DROPSHADOW;
			else
				dwStyle &= ~CS_DROPSHADOW;
			return ::SetClassLong(m_hWND, GCL_STYLE, dwStyle);
		}
		BOOL TinyVisualWND::SetSize(INT cx, INT cy) throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::SetWindowPos(m_hWND, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		BOOL TinyVisualWND::SetPosition(INT x, INT y) throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::SetWindowPos(m_hWND, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		BOOL TinyVisualWND::SetSize(const TinySize& size) throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::SetWindowPos(m_hWND, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		BOOL TinyVisualWND::SetPosition(const TinyPoint& pos) throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::SetWindowPos(m_hWND, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		BOOL TinyVisualWND::ClientToScreen(LPPOINT lpPoint) const throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::ClientToScreen(m_hWND, lpPoint);
		}

		BOOL TinyVisualWND::ClientToScreen(LPRECT lpRect) const throw()
		{
			ASSERT(::IsWindow(m_hWND));
			if (!::ClientToScreen(m_hWND, (LPPOINT)lpRect))
				return FALSE;
			return ::ClientToScreen(m_hWND, ((LPPOINT)lpRect) + 1);
		}

		BOOL TinyVisualWND::ScreenToClient(LPPOINT lpPoint) const throw()
		{
			ASSERT(::IsWindow(m_hWND));
			return ::ScreenToClient(m_hWND, lpPoint);
		}

		BOOL TinyVisualWND::ScreenToClient(LPRECT lpRect) const throw()
		{
			ASSERT(::IsWindow(m_hWND));
			if (!::ScreenToClient(m_hWND, (LPPOINT)lpRect))
				return FALSE;
			return ::ScreenToClient(m_hWND, ((LPPOINT)lpRect) + 1);
		}
		void TinyVisualWND::SetDefaultFont()
		{
			::SendMessage(m_hWND, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
		}
		void TinyVisualWND::SetFont(HFONT hFont, BOOL bRedraw)
		{
			ASSERT(::IsWindow(m_hWND));
			::SendMessage(m_hWND, WM_SETFONT, (WPARAM)hFont, bRedraw);
		}
		HFONT TinyVisualWND::GetFont() const
		{
			ASSERT(::IsWindow(m_hWND));
			return (HFONT)::SendMessage(m_hWND, WM_GETFONT, 0, 0);
		}
	}
}
