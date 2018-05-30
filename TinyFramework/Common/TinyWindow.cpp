#include "../stdafx.h"
#include "TinyWindow.h"
#include "TinyApplication.h"

namespace TinyFramework
{
	IMPLEMENT_DYNAMIC(TinyWindow, TinyHandleHWND);

	TinyWindow::TinyWindow()
		:m_hPrimaryProc(::DefWindowProc)
	{
	};
	TinyWindow::TinyWindow(INT x, INT y, INT cx, INT cy) :
		m_hPrimaryProc(::DefWindowProc)
	{
		Create(NULL, x, y, cx, cy);
	};
	TinyWindow::~TinyWindow()
	{
	};
	LRESULT TinyWindow::DefWindowProc()
	{
		const TinyMsg* pMsg = m_pCurrentMsg;
		LRESULT lRes = 0;
		if (pMsg != NULL)
			lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
		return lRes;
	}
	LRESULT TinyWindow::DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#ifdef STRICT
		return ::CallWindowProc(m_hPrimaryProc, m_hWND, uMsg, wParam, lParam);
#else
		return ::CallWindowProc((FARPROC)m_hPrimaryProc, m_hWND, uMsg, wParam, lParam);
#endif
	}
	LPCSTR TinyWindow::RetrieveClassName()
	{
		return TEXT("TinyWindow");
	}
	HICON TinyWindow::RetrieveIcon()
	{
		return NULL;
	}
	LPCTSTR TinyWindow::RetrieveTitle()
	{
		return TEXT("TinyWindow");
	}
	DWORD TinyWindow::RetrieveStyle()
	{
		return 0;
	}
	DWORD TinyWindow::RetrieveExStyle()
	{
		return 0;
	}
	LPCSTR TinyWindow::RetrieveClass(LPCTSTR lpszClass)
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
				class_ex.lpfnWndProc = TinyWindow::BeginLoop;
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
	BOOL TinyWindow::PreCreateWindow(CREATESTRUCT& cs)
	{
		return TRUE;
	}
	BOOL TinyWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
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
		if (!PreCreateWindow(cs))
			return FALSE;
		LPCTSTR lpszClass = RetrieveClass(cs.lpszClass);
		HWND hWND = ::CreateWindowEx(cs.dwExStyle, lpszClass, cs.lpszName,
			cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent,
			cs.hMenu, cs.hInstance, cs.lpCreateParams);
		return Attach(hWND);
	};
	BOOL TinyWindow::DestroyWindow()
	{
		return ::DestroyWindow(Detach());
	}
	void TinyWindow::PreSubclassWindow()
	{

	}
	BOOL TinyWindow::SubclassWindow(HWND hWND)
	{
		ASSERT(::IsWindow(hWND));
		PreSubclassWindow();
		if (!m_thunk.Initialize(TinyWindow::EndLoop, this))
			return FALSE;
		WNDPROC hProc = m_thunk.GetWNDPROC();
		WNDPROC hOldProc = (WNDPROC)::SetWindowLongPtr(hWND, GWLP_WNDPROC, (LONG_PTR)hProc);
		if (hOldProc == NULL)
			return FALSE;
		m_hPrimaryProc = hOldProc;
		return Attach(hWND);
	};
	void TinyWindow::PreSubclassDlgItem()
	{

	}
	BOOL TinyWindow::SubclassDlgItem(UINT nID, HWND hDlg)
	{
		ASSERT(::IsWindow(hDlg));
		PreSubclassDlgItem();
		HWND hItem = ::GetDlgItem(hDlg, nID);
		if (hItem == NULL)
			return FALSE;
		return SubclassWindow(hItem);
	}
	HWND TinyWindow::UnsubclassWindow(BOOL bForce /* = FALSE */)
	{
		ASSERT(m_hWND != NULL);
		WNDPROC hProc = m_thunk.GetWNDPROC();
		WNDPROC pActiveProc = (WNDPROC)::GetWindowLongPtr(m_hWND, GWLP_WNDPROC);
		HWND hWND = NULL;
		if (bForce || hProc == pActiveProc)
		{
			if (!::SetWindowLongPtr(m_hWND, GWLP_WNDPROC, (LONG_PTR)m_hPrimaryProc))
				return NULL;
			m_hPrimaryProc = ::DefWindowProc;
			hWND = Detach();
		}
		return hWND;
	}
	LRESULT TinyWindow::BeginLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyWindow *_this = NULL;
		if (uMsg == WM_NCCREATE)
		{
			ASSERT(lParam != NULL);
			_this = (TinyWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			if (_this != NULL)
			{
				_this->m_hWND = hWND;
				_this->m_thunk.Initialize(TinyWindow::EndLoop, _this);
				WNDPROC hProc = _this->m_thunk.GetWNDPROC();
				WNDPROC hOldProc = (WNDPROC)::SetWindowLongPtr(hWND, GWLP_WNDPROC, (LONG_PTR)hProc);
				if (hOldProc != BeginLoop)
				{
					ASSERT(_T("HOOK子类化失败.\n"));
				}
				return hProc(hWND, uMsg, wParam, lParam);
			}
		}
		return ::DefWindowProc(hWND, uMsg, wParam, lParam);
	};
	LRESULT TinyWindow::EndLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		TinyWindow* _this = NULL;
		ASSERT(hWND);
		_this = (TinyWindow*)hWND;
		if (_this != NULL)
		{
			TinyMsg msg(_this->m_hWND, uMsg, wParam, lParam);
			const TinyMsg* pOldMsg = _this->m_pCurrentMsg;
			_this->m_pCurrentMsg = &msg;
			LRESULT lRes = 0;
			BOOL bRes = _this->ProcessWindowMessage(_this->m_hWND, uMsg, wParam, lParam, lRes);
			ASSERT(_this->m_pCurrentMsg == &msg);
			if (!bRes)//如果没有设置Handle,调用CallWindowProc处理
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
					if (_this->m_hPrimaryProc != ::DefWindowProc && ::GetWindowLongPtr(_this->m_hWND, GWLP_WNDPROC) == hProc)
					{
						::SetWindowLongPtr(_this->m_hWND, GWLP_WNDPROC, (LONG_PTR)_this->m_hPrimaryProc);
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
	void TinyWindow::OnFinalMessage(HWND hWnd)
	{

	}
	BOOL TinyWindow::PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}
};
