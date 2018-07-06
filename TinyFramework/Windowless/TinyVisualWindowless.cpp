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
			m_bits(NULL),
			m_hOldBitmap(NULL)
		{
			ASSERT(m_hWND);
			HDC hDC = ::GetWindowDC(m_hWND);
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
				if (m_hOldBitmap != NULL)
				{
					::SelectObject(m_hMemDC, m_hOldBitmap);
					m_hOldBitmap = NULL;
				}
				if (m_hMemDC != NULL)
				{
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				HDC hDC = Detach();
				::ReleaseDC(m_hWND, hDC);
			}
		}
		BOOL TinyVisualDC::IsEmpty() const
		{
			return m_hMemDC == NULL || m_hBitmap == NULL;
		}
		TinySize TinyVisualDC::GetSize() const
		{
			return m_size;
		}
		void TinyVisualDC::SetSize(INT cx, INT cy)
		{
			ASSERT(m_hDC);
			if (m_size.cx != cx || m_size.cy != cy)
			{
				m_size.cx = cx;
				m_size.cy = cy;
				SAFE_DELETE_OBJECT(m_hBitmap);
				if (m_hOldBitmap != NULL)
				{
					::SelectObject(m_hMemDC, m_hOldBitmap);
					m_hOldBitmap = NULL;
				}
				if (m_hMemDC != NULL)
				{
					::DeleteDC(m_hMemDC);
					m_hMemDC = NULL;
				}
				m_hMemDC = ::CreateCompatibleDC(m_hDC);
				if (GetWindowLong(m_hWND, GWL_EXSTYLE) & WS_EX_LAYERED)
				{
					BITMAPINFO bi = { 0 };
					bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bi.bmiHeader.biWidth = m_size.cx;
					bi.bmiHeader.biHeight = m_size.cy;
					bi.bmiHeader.biPlanes = 1;
					bi.bmiHeader.biBitCount = 32;
					bi.bmiHeader.biCompression = BI_RGB;
					bi.bmiHeader.biSizeImage = ((m_size.cx * 32 + 31) / 32) * m_size.cy * 4;
					m_bits = NULL;
					m_hBitmap = CreateDIBSection(m_hDC, &bi, DIB_RGB_COLORS, (void **)&m_bits, NULL, NULL);
					if (m_hBitmap != NULL)
					{
						m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
					}
				}
				else
				{
					m_bits = NULL;
					m_hBitmap = CreateCompatibleBitmap(m_hDC, m_size.cx, m_size.cy);
					if (m_hBitmap != NULL)
					{
						m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
					}
				}
			}
		}
		HDC	 TinyVisualDC::GetCompatibleDC() const
		{
			return m_hMemDC;
		}
		HBITMAP	 TinyVisualDC::GetCompatibleBitmap() const
		{
			return m_hBitmap;
		}
		TinySize TinyVisualDC::GetTextExtent(HDC hDC, LPCSTR lpszString, INT count)
		{
			TinySize size;
			GetTextExtentPoint32(hDC, lpszString, count, &size);
			return size;
		}
		BOOL TinyVisualDC::Render(INT dstX, INT dstY, INT dstCX, INT dstCY, INT srcX, INT srcY)
		{
			if (!m_hMemDC || !m_hBitmap)
				return FALSE;
			return ::BitBlt(m_hDC, dstX, dstY, dstCX, dstCY, m_hMemDC, srcX, srcY, SRCCOPY);
		}
		BOOL TinyVisualDC::Render(INT dstX, INT dstY, INT dstCX, INT dstCY, INT srcX, INT srcY, INT srcCX, INT srcCY)
		{
			if (!m_hMemDC || !m_hBitmap)
				return FALSE;
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			return ::AlphaBlend(m_hDC, dstX, dstY, dstCX, dstCY, m_hMemDC, srcX, srcY, srcCX, srcCY, bs);
		}
		BOOL TinyVisualDC::RenderLayer()
		{
			if (!m_hMemDC || !m_hBitmap)
				return FALSE;
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			TinyRectangle windowRect;
			::GetWindowRect(m_hWND, &windowRect);
			TinyPoint dstPos = windowRect.Position();
			TinySize  dstSize = windowRect.Size();
			TinyPoint pos;
			return ::UpdateLayeredWindow(m_hWND, m_hDC, &dstPos, &dstSize, m_hMemDC, &pos, 0, &bs, ULW_ALPHA);
		}
		BOOL TinyVisualDC::Save(LPCSTR pzFile)
		{
			return SaveBitmapToFile1(m_hBitmap, pzFile);
		}
		void TinyVisualDC::Clear()
		{
			if (m_bits != NULL)
			{
				ZeroMemory(m_bits, ((m_size.cx * 32 + 31) / 32) * m_size.cy * 4);
			}
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
			SetBrush(((HBRUSH)SelectObject((m_hDC), (HGDIOBJ)(HBRUSH)(NULL_BRUSH))));
			SetPen(((HPEN)SelectObject((m_hDC), (HGDIOBJ)(HPEN)(NULL_PEN))));
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
			m_bAllowTracking(TRUE),
			m_owner(NULL)
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
			return TEXT("VISUALWINDOWLESS");
		}
		LPCSTR TinyVisualWindowless::RetrieveTitle()
		{
			return TEXT("VISUALWINDOWLESS");
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
				if (!m_shadow.Create(m_hWND, 0, 0, 0, 0))
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
				m_shadow.DestroyWindow();
			}
			m_document.Uninitialize();
		}
		void TinyVisualWindowless::Draw(RECT *lpRect)
		{
			if (m_visualDC != NULL)
			{
				TinyRectangle s;
				s.SetSize(m_visualDC->GetSize());
				m_document.Draw(m_visualDC, lpRect == NULL ? s : *lpRect);
			}
		}
		BOOL TinyVisualWindowless::AddFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Add(ps);
		}
		BOOL TinyVisualWindowless::RemoveFilter(TinyVisualFilter* ps)
		{
			return m_mFilters.Remove(ps);
		}
		void TinyVisualWindowless::SetOwner(TinyVisualWindowless* windowless)
		{
			m_owner = windowless;
		}
		void TinyVisualWindowless::AllowTracking(BOOL bAllow)
		{
			m_bAllowTracking = bAllow;
		}
		TinyVisualWindowless*	TinyVisualWindowless::GetOwner()
		{
			return m_owner;
		}
		TinyVisualShadow& TinyVisualWindowless::GetShadow()
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
					m_shadow.DrawShadow();
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
					m_document.Redraw();
				}
			}
			if (m_shadow != NULL)
			{
				TinyRectangle box = m_shadow.GetShadowRectangle();
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
				TinyRectangle box = m_shadow.GetShadowRectangle();
				INT x = m_position.x - box.left;
				INT y = m_position.y - box.top;
				::SetWindowPos(m_shadow.Handle(), NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
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
					tme.dwFlags = TME_LEAVE | TME_HOVER;
					tme.dwHoverTime = 10;
					m_bMouseTracking = _TrackMouseEvent(&tme);
				}
			}
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnMouseMove(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT	TinyVisualWindowless::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_bAllowTracking)
			{
				if (m_bMouseTracking)
				{
					m_bMouseTracking = FALSE;
				}
			}
			TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			LRESULT lRes = m_document.OnMouseHover(pos, static_cast<DWORD>(wParam));
			bHandled = IsMsgHandled();
			return lRes;
		}
		LRESULT TinyVisualWindowless::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			if (m_bAllowTracking)
			{
				if (m_bMouseTracking)
				{
					m_bMouseTracking = FALSE;
				}
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
				::ShowWindow(m_shadow.Handle(), !wParam ? SW_HIDE : SW_SHOWNA);
			}
			return FALSE;
		}
		LRESULT TinyVisualWindowless::OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (m_shadow != NULL)
			{
				if (IsWindowVisible(m_hWND))
				{
					m_shadow.DrawShadow();
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
		LRESULT TinyVisualWindowless::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
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
