#include "stdafx.h"
#include "Snapshot.h"

namespace WindowCapture
{
	Snapshot::Snapshot()
		:m_hBBitmap(NULL),
		m_hFBitmap(NULL),
		m_hOldBBitmap(NULL),
		m_hOldFBitmap(NULL),
		m_hBDC(NULL),
		m_hFDC(NULL)
	{

	}
	Snapshot::~Snapshot()
	{
	}
	BOOL Snapshot::Create(HWND hParent)
	{
		TinyRectangle s;
		::GetWindowRect(::GetDesktopWindow(), &s);
		return TinyControl::Create(hParent, 0, 0, s.Width(), s.Height(), FALSE);
	}
	DWORD Snapshot::RetrieveStyle()
	{
		return WS_THICKFRAME;
	}
	DWORD Snapshot::RetrieveExStyle()
	{
		return (WS_EX_TOOLWINDOW);
	}
	LPCSTR Snapshot::RetrieveClassName()
	{
		return TEXT("Snapshot");
	}
	LPCSTR Snapshot::RetrieveTitle()
	{
		return TEXT("Snapshot");
	}
	HICON Snapshot::RetrieveIcon()
	{
		return NULL;
	}
	BOOL Snapshot::Initialize()
	{
		HWND hDesktop = ::GetDesktopWindow();
		if (!hDesktop)
			return FALSE;
		HDC  hDesktopDC = ::GetDC(hDesktop);
		if (!hDesktopDC)
			return FALSE;
		RECT s = { 0 };
		if (!::GetWindowRect(hDesktop, &s))
		{
			ReleaseDC(hDesktop, hDesktopDC);
			return FALSE;
		}
		m_hBDC = CreateCompatibleDC(hDesktopDC);
		ASSERT(m_hBDC);
		m_hFDC = CreateCompatibleDC(hDesktopDC);
		ASSERT(m_hFDC);
		m_hBBitmap = CreateDiscardableBitmap(hDesktopDC, TO_CX(s), TO_CY(s));
		ASSERT(m_hBBitmap);
		m_hFBitmap = CreateDiscardableBitmap(hDesktopDC, TO_CX(s), TO_CY(s));
		ASSERT(m_hFBitmap);
		m_hOldBBitmap = (HBITMAP)::SelectObject(m_hBDC, m_hBBitmap);
		ASSERT(m_hOldBBitmap);
		m_hOldFBitmap = (HBITMAP)::SelectObject(m_hFDC, m_hFBitmap);
		ASSERT(m_hOldFBitmap);
		BLENDFUNCTION bs;
		bs.AlphaFormat = AC_SRC_OVER;
		bs.BlendFlags = 0;
		bs.BlendOp = AC_SRC_OVER;
		bs.SourceConstantAlpha = 128;
		if (::AlphaBlend(m_hBDC, 0, 0, TO_CX(s), TO_CY(s), hDesktopDC, 0, 0, TO_CX(s), TO_CY(s), bs))
		{
			if (::BitBlt(m_hFDC, 0, 0, TO_CX(s), TO_CY(s), hDesktopDC, 0, 0, SRCCOPY))
			{
				return ::ReleaseDC(hDesktop, hDesktopDC);
			}
		}
		return FALSE;
	}
	void Snapshot::Uninitialize()
	{
		if (m_hBDC && m_hOldBBitmap)
		{
			SelectObject(m_hBDC, m_hOldBBitmap);
			m_hOldBBitmap = NULL;
		}
		if (m_hFDC && m_hOldFBitmap)
		{
			SelectObject(m_hFDC, m_hOldFBitmap);
			m_hOldFBitmap = NULL;
		}
		SAFE_DELETE_OBJECT(m_hBBitmap);
		SAFE_DELETE_OBJECT(m_hFBitmap);
	}
	LRESULT Snapshot::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		Initialize();
		return FALSE;
	}
	LRESULT Snapshot::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		Uninitialize();
		return FALSE;
	}
	LRESULT Snapshot::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT Snapshot::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m_hWND, &ps);
		{
			RECT s = { 0 };
			::GetWindowRect(m_hWND, &s);
			POINT pos;
			::GetCursorPos(&pos);
			TinyMemDC dc(hDC, TO_CX(s), TO_CY(s));
			HBRUSH  hOldBrush = (HBRUSH)dc.SelectObject(GetStockObject(NULL_BRUSH));
			::BitBlt(dc, 0, 0, TO_CX(s), TO_CY(s), m_hBDC, 0, 0, SRCCOPY);
			if (::GetAsyncKeyState(VK_LBUTTON))
			{
				TinyRectangle rectangle;
				rectangle.left = m_downPos.x;
				rectangle.top = m_downPos.y;
				rectangle.right = m_mousePos.x < TO_CX(s) ? m_mousePos.x : TO_CX(s);
				rectangle.bottom = m_mousePos.y < TO_CY(s) ? m_mousePos.y : TO_CY(s);
				if (rectangle.left > rectangle.right)
				{
					rectangle.left = rectangle.right + rectangle.left;
					rectangle.right = rectangle.left - rectangle.right;
					rectangle.left = rectangle.left - rectangle.right;
				}
				if (rectangle.top > rectangle.bottom)
				{
					rectangle.top = rectangle.top + rectangle.bottom;
					rectangle.bottom = rectangle.top - rectangle.bottom;
					rectangle.top = rectangle.top - rectangle.bottom;
				}
				::BitBlt(dc, rectangle.left, rectangle.top, abs(rectangle.right - rectangle.left), abs(rectangle.bottom - rectangle.top), m_hFDC, rectangle.left, rectangle.top, SRCCOPY);
				::Rectangle(dc, rectangle.left - 1, rectangle.top - 1, rectangle.right + 1, rectangle.bottom + 1);
			}
			::StretchBlt(dc, pos.x + 20, pos.y + 20, 120, 80, m_hFDC, pos.x - 15, pos.y - 10, 30, 20, SRCCOPY);
			TinyPen pen;
			pen.CreatePen(PS_SOLID, 5, RGB(0, 174, 255));
			HPEN hOldPen = (HPEN)::SelectObject(dc, pen);
			::Rectangle(dc, pos.x + 20, pos.y + 60, pos.x + 140, pos.y + 61);
			::Rectangle(dc, pos.x + 80, pos.y + 20, pos.x + 81, pos.y + 100);
			::SelectObject(dc, hOldPen);
			::Rectangle(dc, pos.x + 20, pos.y + 20, pos.x + 140, pos.y + 140);
			::SetTextColor(dc, RGB(255, 255, 255));
			::SetBkColor(dc, RGB(128, 128, 128));

			dc.Render(s, s, FALSE);
		}
		EndPaint(m_hWND, &ps);
		return FALSE;
	}
	LRESULT Snapshot::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}

	LRESULT Snapshot::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_mousePos.x = LOWORD(lParam);
		m_mousePos.y = HIWORD(lParam);
		InvalidateRect(m_hWND, NULL, FALSE);
		return FALSE;
	}

	LRESULT Snapshot::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_downPos.x = LOWORD(lParam);
		m_downPos.y = HIWORD(lParam);
		SetCapture(m_hWND);
		return FALSE;
	}

	LRESULT Snapshot::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		ReleaseCapture();
		return FALSE;
	}

	LRESULT Snapshot::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}