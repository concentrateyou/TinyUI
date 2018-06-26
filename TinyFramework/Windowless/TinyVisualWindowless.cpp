#include "../stdafx.h"
#include "TinyVisualWindowless.h"
#include "../Common/TinyLogging.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualDC, TinyHandleHDC);
		TinyVisualDC::TinyVisualDC(HWND hWND)
			:m_hWND(hWND),
			m_hMemDC(NULL),
			m_hBitmap(NULL),
			m_hOldBitmap(NULL)
		{
			ASSERT(m_hWND);
			HDC hDC = ::GetDC(m_hWND);
			Attach(hDC);
			SetGraphicsMode(hDC, GM_ADVANCED);
			SetBkMode(hDC, TRANSPARENT);
			RECT s = { 0 };
			GetWindowRect(m_hWND, &s);
			SetSize(TO_CX(s), TO_CY(s));
		}
		TinyVisualDC::~TinyVisualDC()
		{
			ASSERT(m_hWND);
			if (m_hDC != NULL)
			{
				SAFE_DELETE_OBJECT(m_hBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_hOldBitmap != NULL)
					{
						::SelectObject(m_hMemDC, m_hOldBitmap);
						m_hOldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				HDC hDC = Detach();
				::ReleaseDC(m_hWND, hDC);
			}
		}
		void TinyVisualDC::SetSize(INT cx, INT cy)
		{
			ASSERT(m_hDC);
			if (m_size.cx != cx || m_size.cy != cy)
			{
				m_size.cx = cx;
				m_size.cy = cy;
				SAFE_DELETE_OBJECT(m_hBitmap);
				if (m_hMemDC != NULL)
				{
					if (m_hOldBitmap != NULL)
					{
						SelectObject(m_hMemDC, m_hOldBitmap);
						m_hOldBitmap = NULL;
					}
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				m_hMemDC = ::CreateCompatibleDC(m_hDC);
				m_hBitmap = ::CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
				m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
			}
		}
		HDC	 TinyVisualDC::GetMemDC() const
		{
			return m_hMemDC;
		}
		BOOL TinyVisualDC::Render(const RECT& s)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, s.left, s.top, SRCCOPY);
		}
		BOOL TinyVisualDC::Render(const RECT& s, INT x, INT y)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			return ::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, x, y, SRCCOPY);
		}
		BOOL TinyVisualDC::RenderLayer(const RECT& s)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			if (::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, s.left, s.top, SRCCOPY))
			{
				HDC hScreenDC = GetDC(NULL);
				BLENDFUNCTION blendFunction;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;
				blendFunction.BlendFlags = 0;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.SourceConstantAlpha = 255;
				RECT wndRect;
				::GetWindowRect(m_hWND, &wndRect);
				POINT ptSrc = { 0,0 };
				SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
				UpdateLayeredWindow(m_hWND, hScreenDC, &ptSrc, &wndSize, m_hMemDC, &ptSrc, 0, &blendFunction, 2);
				ReleaseDC(NULL, hScreenDC);
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyVisualDC::RenderLayer(const RECT& s, INT x, INT y)
		{
			if (!m_hMemDC || !m_hBitmap) return FALSE;
			if (::BitBlt(m_hDC, s.left, s.top, TO_CX(s), TO_CY(s), m_hMemDC, x, y, SRCCOPY))
			{
				HDC hScreenDC = GetDC(NULL);
				BLENDFUNCTION blendFunction;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;
				blendFunction.BlendFlags = 0;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.SourceConstantAlpha = 255;
				RECT wndRect;
				::GetWindowRect(m_hWND, &wndRect);
				POINT ptSrc = { 0,0 };
				SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
				UpdateLayeredWindow(m_hWND, hScreenDC, &ptSrc, &wndSize, m_hMemDC, &ptSrc, 0, &blendFunction, 2);
				ReleaseDC(NULL, hScreenDC);
				return TRUE;
			}
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyClipCanvas::TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint)
			:TinyCanvas(hDC),
			m_hRGN(NULL),
			m_hOldRGN(NULL)
		{
			ASSERT(spvis);
			RECT clip = { 0 };
			::GetClipBox(hDC, &clip);
			m_hOldRGN = ::CreateRectRgnIndirect(&clip);
			m_hRGN = ::CreateRectRgnIndirect(&rcPaint);
			if (spvis->GetClip())
			{
				::CombineRgn(m_hRGN, m_hRGN, spvis->GetClip(), RGN_AND);
			}
			::ExtSelectClipRgn(m_hDC, m_hRGN, RGN_AND);
		}
		TinyClipCanvas::~TinyClipCanvas()
		{
			::SelectClipRgn(m_hDC, m_hOldRGN);
			SAFE_DELETE_OBJECT(m_hOldRGN);
			SAFE_DELETE_OBJECT(m_hRGN);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualFilters::TinyVisualFilters()
		{

		}
		TinyVisualFilters::~TinyVisualFilters()
		{
		}
		BOOL TinyVisualFilters::Add(TinyVisualFilter*  element)
		{
			return m_array.Add(element);
		}
		BOOL TinyVisualFilters::Remove(TinyVisualFilter* element)
		{
			return m_array.Remove(element);
		}
		BOOL TinyVisualFilters::RemoveAt(INT index)
		{
			return m_array.RemoveAt(index);
		}
		void TinyVisualFilters::RemoveAll()
		{
			m_array.RemoveAll();
		}
		TinyVisualFilter* TinyVisualFilters::operator[](INT index)
		{
			return m_array[index];
		}
		const TinyVisualFilter* TinyVisualFilters::operator[](INT index) const
		{
			return m_array[index];
		}
		INT TinyVisualFilters::Lookup(TinyVisualFilter* element) const
		{
			return m_array.Lookup(element);
		}
		INT  TinyVisualFilters::GetSize() const
		{
			return m_array.GetSize();
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNAMIC(TinyVisualWindowless, TinyVisualWND);
		TinyVisualWindowless::TinyVisualWindowless()
			:m_document(*this),
			m_visualDC(NULL),
			m_bMouseTracking(FALSE),
			m_bAllowTracking(TRUE)
		{

		}
		TinyVisualWindowless::~TinyVisualWindowless()
		{

		}
		BOOL TinyVisualWindowless::Create(HWND hParent, LPCSTR pszFile)
		{
			if (!TinyVisualWND::Create(hParent, 0, 0, 0, 0))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualWindowless::Create(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile)
		{
			if (!TinyVisualWND::Create(hParent, lpTemplateName))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualWindowless::Create(HWND hParent, WORD wInteger, LPCSTR pszFile)
		{
			if (!TinyVisualWND::Create(hParent, wInteger))
				return FALSE;
			return TRUE;
		}
		INT_PTR TinyVisualWindowless::DoModal(HWND hParent, WORD wInteger, LPCSTR pszFile)
		{
			return TinyVisualWND::DoModal(hParent, wInteger);
		}
		INT_PTR TinyVisualWindowless::DoModal(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile)
		{
			return TinyVisualWND::DoModal(hParent, lpTemplateName);
		}
		DWORD TinyVisualWindowless::RetrieveStyle()
		{
			return (WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_MINIMIZEBOX);
		}
		DWORD TinyVisualWindowless::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_OVERLAPPEDWINDOW);
		}
		LPCSTR TinyVisualWindowless::RetrieveClassName()
		{
			return TEXT("TinyVisualHWND");
		}
		LPCSTR TinyVisualWindowless::RetrieveTitle()
		{
			return TEXT("TinyVisualHWND");
		}
		HICON TinyVisualWindowless::RetrieveIcon()
		{
			return NULL;
		}
		TinyVisualDocument&	TinyVisualWindowless::GetDocument()
		{
			return m_document;
		}
		BOOL TinyVisualWindowless::Initialize()
		{
			m_visualDC.Reset(new TinyVisualDC(m_hWND));
			if (!m_visualDC)
			{
				LOG(ERROR) << "Create TinyVisualDC FAIL";
				return FALSE;
			}
			//创建阴影窗口
			if (!(RetrieveExStyle() & WS_EX_LAYERED))
			{
				m_shadow.Reset(new TinyVisualShadow());
				ASSERT(m_shadow);
				if (!m_shadow->Create(m_hWND, 0, 0, 0, 0))
				{
					LOG(ERROR) << "Create TinyVisualShadow FAIL";
					return FALSE;
				}
			}
			if (!m_document.Initialize())
			{
				LOG(ERROR) << "Document Initialize FAIl";
				return FALSE;
			}
			this->OnInitialize();
			return TRUE;
		}
		void TinyVisualWindowless::Uninitialize()
		{
			m_visualDC.Reset(NULL);
			if (m_shadow != NULL)
			{
				m_shadow->DestroyWindow();
			}
			m_shadow.Reset(NULL);
			m_document.Uninitialize();
		}
		BOOL TinyVisualWindowless::AddFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Add(ps);
		}
		BOOL TinyVisualWindowless::RemoveFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Remove(ps);
		}
		void TinyVisualWindowless::AllowTracking(BOOL bAllow)
		{
			m_bAllowTracking = bAllow;
		}
		TinyVisualShadow* TinyVisualWindowless::GetShadow()
		{
			return m_shadow;
		}
		BOOL TinyVisualWindowless::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
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
		LRESULT TinyVisualWindowless::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			PAINTSTRUCT ps = { 0 };
			HDC hDC = BeginPaint(m_hWND, &ps);
			m_document.Draw(m_visualDC, ps.rcPaint);
			EndPaint(m_hWND, &ps);
			if (IsWindowVisible(m_hWND))
			{
				if (m_shadow != NULL)
				{
					m_shadow->DrawShadow();
				}
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return TRUE;
		}
		LRESULT TinyVisualWindowless::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (lParam != NULL)
			{
				m_size.cx = LOWORD(lParam);
				m_size.cy = HIWORD(lParam);
				if (m_visualDC != NULL)
				{
					m_visualDC->SetSize(m_size.cx, m_size.cy);
					m_document.OnSize(m_size);
					::RedrawWindow(m_hWND, NULL, NULL, RDW_INVALIDATE);
				}
			}
			if (m_shadow != NULL)
			{
				ASSERT(m_shadow->Handle());
				TinyRectangle box = m_shadow->GetShadowBox();
				INT cx = m_size.cx + box.left + box.right;
				INT cy = m_size.cy + box.top + box.bottom;
				::SetWindowPos(m_shadow->Handle(), NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
				if (IsWindowVisible(m_hWND))
				{
					m_shadow->DrawShadow();
				}
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (lParam != NULL)
			{
				m_position.x = LOWORD(lParam);
				m_position.y = HIWORD(lParam);
			}
			if (m_shadow != NULL)
			{
				ASSERT(m_shadow->Handle());
				TinyRectangle box = m_shadow->GetShadowBox();
				INT x = m_position.x - box.left;
				INT y = m_position.y - box.top;
				::SetWindowPos(m_shadow->Handle(), NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			PostQuitMessage(0);//退出应用程序
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnCreate(uMsg, wParam, lParam, bHandled);
			if (!Initialize())
				PostQuitMessage(0);//直接退出
			return bRes;
		}
		LRESULT TinyVisualWindowless::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnDestory(uMsg, wParam, lParam, bHandled);
			OnUninitialize();
			Uninitialize();
			return bRes;
		}
		LRESULT TinyVisualWindowless::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			BOOL bRes = TinyVisualWND::OnInitDialog(uMsg, wParam, lParam, bHandled);
			if (!Initialize())
				PostQuitMessage(0);//直接退出
			return bRes;
		}
		LRESULT TinyVisualWindowless::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
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
			LRESULT lRes = m_document.OnMouseMove(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_bAllowTracking)
			{
				if (m_bMouseTracking)
					m_bMouseTracking = FALSE;
			}
			LRESULT lRes = m_document.OnMouseLeave();
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			::ScreenToClient(m_hWND, &pos);
			LRESULT lRes = m_document.OnMouseWheel(pos, GET_WHEEL_DELTA_WPARAM(wParam), GET_KEYSTATE_WPARAM(wParam));
			bHandled = IsMsgHandled();
			return lRes;
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnLButtonDown(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnLButtonUp(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnLButtonDBClick(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnRButtonDown(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnRButtonUp(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnRButtonDBClick(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnMButtonDown(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnMButtonUp(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnMButtonDBClick(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnKeyDown(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnKeyUp(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnChar(static_cast<DWORD>(wParam), LOWORD(lParam), HIWORD(lParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnSetCursor(reinterpret_cast<HWND>(wParam), LOWORD(lParam), HIWORD(lParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnSetFocus(reinterpret_cast<HWND>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			LRESULT lRes = m_document.OnKillFocus(reinterpret_cast<HWND>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_shadow != NULL)
			{
				ASSERT(m_shadow->Handle());
				::ShowWindow(m_shadow->Handle(), !wParam ? SW_HIDE : SW_SHOWNA);
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_shadow != NULL)
			{
				ASSERT(m_shadow->Handle());
				if (IsWindowVisible(m_hWND))
				{
					m_shadow->DrawShadow();
				}
			}

			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
					m_shadow->ShowWindow(SW_HIDE);
					m_shadow->DrawShadow();
				}
				if (wParam == SC_RESTORE && !IsZoomed(m_hWND))
				{
					m_shadow->ShowWindow(SW_SHOW);
					m_shadow->DrawShadow();
				}
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
		LRESULT TinyVisualWindowless::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
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
			if (m_document.GetParent(NULL) == m_document.GetVisualByPos(pos.x, pos.y))
				return HTCAPTION;
			return HTCLIENT;
		}
		LRESULT TinyVisualWindowless::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			MINMAXINFO* ps = (MINMAXINFO*)lParam;
			TinyVisual* spvis = m_document.GetParent(NULL);
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
			return TRUE;
		}
	}
}
