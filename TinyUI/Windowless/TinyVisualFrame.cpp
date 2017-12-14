#include "../stdafx.h"
#include "TinyVisualFrame.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualFrame, TinyVisualWND);
		TinyVisualFrame::TinyVisualFrame()
			:m_document(NULL),
			m_visualDC(NULL),
			m_bMouseTracking(FALSE),
			m_bAllowTracking(TRUE)
		{

		}
		TinyVisualFrame::~TinyVisualFrame()
		{

		}
		BOOL TinyVisualFrame::Create(HWND hParent, LPCSTR pszFile)
		{
			m_szSkinFile = pszFile;
			if (!TinyVisualWND::Create(hParent, 0, 0, 0, 0))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualFrame::Create(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile)
		{
			m_szSkinFile = pszFile;
			if (!TinyVisualWND::Create(hParent, lpTemplateName))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualFrame::Create(HWND hParent, WORD wInteger, LPCSTR pszFile)
		{
			m_szSkinFile = pszFile;
			if (!TinyVisualWND::Create(hParent, wInteger))
				return FALSE;
			return TRUE;
		}
		INT_PTR TinyVisualFrame::DoModal(HWND hParent, WORD wInteger, LPCSTR pszFile)
		{
			m_szSkinFile = pszFile;
			return TinyVisualWND::DoModal(hParent, wInteger);
		}
		INT_PTR TinyVisualFrame::DoModal(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile)
		{
			m_szSkinFile = pszFile;
			return TinyVisualWND::DoModal(hParent, lpTemplateName);
		}
		DWORD TinyVisualFrame::RetrieveStyle()
		{
			return (WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX);
		}
		DWORD TinyVisualFrame::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_OVERLAPPEDWINDOW);
		}
		LPCSTR TinyVisualFrame::RetrieveClassName()
		{
			return TEXT("TinyVisualHWND");
		}
		LPCSTR TinyVisualFrame::RetrieveTitle()
		{
			return TEXT("TinyVisualHWND");
		}
		HICON TinyVisualFrame::RetrieveIcon()
		{
			return NULL;
		}
		TinyVisualDocument*	TinyVisualFrame::GetDocument()
		{
			return m_document;
		}
		BOOL TinyVisualFrame::Initialize()
		{
			if (!PathFileExists(m_szSkinFile.CSTR()))
				return FALSE;
			m_visualDC.Reset(new TinyVisualDC(m_hWND));
			if (!m_visualDC)
				return FALSE;
			m_document.Reset(new TinyVisualDocument(this));
			if (!m_document)
				return FALSE;
			if (!m_builder.LoadFile(m_szSkinFile.CSTR()))
				return FALSE;
			if (!m_document->Initialize(&m_builder))
				return FALSE;
			this->OnInitialize();
			return m_shadow.Create(m_hWND, 0, 0, 0, 0);
		}
		void TinyVisualFrame::Uninitialize()
		{
			if (m_document != NULL)
				m_document->Uninitialize();
			m_document.Reset(NULL);
			m_visualDC.Reset(NULL);
			m_shadow.DestroyWindow();
		}
		BOOL TinyVisualFrame::AddFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Add(ps);
		}
		BOOL TinyVisualFrame::RemoveFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Remove(ps);
		}
		void TinyVisualFrame::AllowTracking(BOOL bAllow)
		{
			m_bAllowTracking = bAllow;
		}
		TinyVisualShadow& TinyVisualFrame::GetShadow()
		{
			return m_shadow;
		}
		BOOL TinyVisualFrame::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
		{
			for (INT i = m_mFilters.GetSize() - 1; i >= 0; i--)
			{
				TinyVisualFilter* pFilter = m_mFilters[i];
				if (pFilter != NULL && pFilter->OnFilter(hWnd, uMsg, wParam, lParam, lResult))
				{
					return TRUE;
				}
			}
			return TinyVisualWND::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
		}
		LRESULT TinyVisualFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_document != NULL)
			{
				PAINTSTRUCT ps = { 0 };
				HDC hDC = BeginPaint(m_hWND, &ps);
				m_document->Draw(m_visualDC, ps.rcPaint);
				EndPaint(m_hWND, &ps);
			}
			if (IsWindowVisible(m_hWND))
			{
				m_shadow.DrawShadow();
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return TRUE;
		}
		LRESULT TinyVisualFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (lParam != NULL)
			{
				m_size.cx = LOWORD(lParam);
				m_size.cy = HIWORD(lParam);
				if (m_visualDC != NULL && m_document != NULL)
				{
					m_visualDC->SetSize(m_size.cx, m_size.cy);
					m_document->OnSize(m_size);
					::RedrawWindow(m_hWND, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
			}
			if (m_shadow != NULL)
			{
				TinyRectangle box = m_shadow.GetShadowBox();
				INT cx = m_size.cx + box.left + box.right;
				INT cy = m_size.cy + box.top + box.bottom;
				::SetWindowPos(m_shadow.Handle(), NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
				if (IsWindowVisible(m_hWND))
				{
					m_shadow.DrawShadow();
				}
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (lParam != NULL)
			{
				m_pos.x = LOWORD(lParam);
				m_pos.y = HIWORD(lParam);
			}
			if (m_shadow != NULL)
			{
				TinyRectangle box = m_shadow.GetShadowBox();
				INT x = m_pos.x - box.left;
				INT y = m_pos.y - box.top;
				::SetWindowPos(m_shadow.Handle(), NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
			return FALSE;
		}

		LRESULT TinyVisualFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			PostQuitMessage(0);//退出应用程序
			return FALSE;
		}

		LRESULT TinyVisualFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnCreate(uMsg, wParam, lParam, bHandled);
			if (!Initialize())
				PostQuitMessage(0);//直接退出
			return bRes;
		}
		LRESULT TinyVisualFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnDestory(uMsg, wParam, lParam, bHandled);
			OnUninitialize();
			Uninitialize();
			return bRes;
		}
		LRESULT TinyVisualFrame::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnInitDialog(uMsg, wParam, lParam, bHandled);
			if (!Initialize())
				PostQuitMessage(0);//直接退出
			return bRes;
		}
		LRESULT TinyVisualFrame::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				if (m_bAllowTracking)
				{
					if (!m_bMouseTracking)
					{
						TRACKMOUSEEVENT tme;
						tme.cbSize = sizeof(tme);
						tme.hwndTrack = m_hWND;
						tme.dwFlags = TME_LEAVE;
						tme.dwHoverTime = 10;
						m_bMouseTracking = _TrackMouseEvent(&tme);
					}
				}
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnMouseMove(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				if (m_bAllowTracking)
				{
					if (m_bMouseTracking)
						m_bMouseTracking = FALSE;
				}
				LRESULT lRes = m_document->OnMouseLeave();
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				::ScreenToClient(m_hWND, &pos);
				LRESULT lRes = m_document->OnMouseWheel(pos, GET_WHEEL_DELTA_WPARAM(wParam), GET_KEYSTATE_WPARAM(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnLButtonDown(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnLButtonUp(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnLButtonDBClick(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnRButtonDown(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnRButtonUp(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnRButtonDBClick(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnMButtonDown(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnMButtonUp(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				LRESULT lRes = m_document->OnMButtonDBClick(pos, static_cast<DWORD>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnKeyDown(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnKeyUp(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnChar(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnSetCursor(reinterpret_cast<HWND>(wParam), LOWORD(lParam), HIWORD(lParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnSetFocus(reinterpret_cast<HWND>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_document != NULL)
			{
				LRESULT lRes = m_document->OnKillFocus(reinterpret_cast<HWND>(wParam));
				bHandled = IsMsgHandled();
				return lRes;
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_shadow != NULL)
			{
				::ShowWindow(m_shadow, !wParam ? SW_HIDE : SW_SHOWNA);
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (IsWindowVisible(m_hWND))
			{
				m_shadow.DrawShadow();
			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (wParam == SC_CLOSE)
			{
				SendMessage(m_hWND, WM_CLOSE, NULL, NULL);
				return FALSE;
			}
			if (m_shadow != NULL)
			{
				//最小化最大化不需要阴影
				if (wParam == SC_MINIMIZE || wParam == SC_MAXIMIZE)
				{
					m_shadow.ShowWindow(SW_HIDE);
					m_shadow.DrawShadow();
				}
				if (wParam == SC_RESTORE && !IsZoomed(m_hWND))
				{
					m_shadow.ShowWindow(SW_SHOW);
					m_shadow.DrawShadow();
				}

			}
			return FALSE;
		}
		LRESULT TinyVisualFrame::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			if (static_cast<BOOL>(wParam))
			{
				NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				ps->rgrc[0].left = ps->lppos->x < 0 ? 0 : ps->lppos->x;
				ps->rgrc[0].top = ps->lppos->y < 0 ? 0 : ps->lppos->y;
				ps->rgrc[0].bottom = ps->rgrc[0].top + ps->lppos->cy;
				ps->rgrc[0].right = ps->rgrc[0].left + ps->lppos->cx;
			}
			return TRUE;
		}
		LRESULT TinyVisualFrame::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			if (m_document != NULL)
			{
				TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				TinyRectangle rectangle;
				this->GetWindowRect(rectangle);
				if (!rectangle.PtInRect(pos))
					return HTNOWHERE;
				INT cx = GetSystemMetrics(SM_CXBORDER);
				INT cy = GetSystemMetrics(SM_CYBORDER);
				if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
					return HTTOPLEFT;
				if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y > (rectangle.top + cy) && pos.y < (rectangle.bottom - cy))
					return HTLEFT;
				if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
					return HTBOTTOMLEFT;
				if (pos.x > (rectangle.left + cx) && pos.x < (rectangle.right - cx) && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
					return HTTOP;
				if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
					return HTTOPRIGHT;
				if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y > (rectangle.top + cy) && pos.y < (rectangle.bottom - cy))
					return HTRIGHT;
				if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
					return HTBOTTOMRIGHT;
				if (pos.x > (rectangle.left + cx) && pos.x < (rectangle.right - cx) && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
					return HTBOTTOM;
				::ScreenToClient(m_hWND, &pos);
				if (m_document->GetParent(NULL) == m_document->GetVisualByPos(pos.x, pos.y))
					return HTCAPTION;
			}
			return HTCLIENT;
		}
		LRESULT TinyVisualFrame::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			if (m_document != NULL)
			{
				MINMAXINFO* ps = (MINMAXINFO*)lParam;
				TinyVisual* spvis = m_document->GetParent(NULL);
				ASSERT(spvis);
				TinySize minSize = spvis->GetMinimumSize();
				if (!minSize.IsEmpty())
				{
					ps->ptMinTrackSize.x = minSize.cx;
					ps->ptMinTrackSize.y = minSize.cy;
				}
				TinySize maxSize = spvis->GetMaximumSize();
				if (!maxSize.IsEmpty())
				{
					ps->ptMaxTrackSize.x = maxSize.cx;
					ps->ptMaxTrackSize.y = maxSize.cy;
				}
				else
				{
					MONITORINFO mi = { 0 };
					mi.cbSize = sizeof(MONITORINFO);
					GetMonitorInfo(MonitorFromWindow(m_hWND, MONITOR_DEFAULTTONEAREST), &mi);
					ps->ptMaxTrackSize.x = abs(mi.rcWork.right - mi.rcWork.left);
					ps->ptMaxTrackSize.y = abs(mi.rcWork.bottom - mi.rcWork.top);
				}
				ps->ptMaxPosition.x = 0;
				ps->ptMaxPosition.y = 0;
			}
			return TRUE;
		}
	}
}
