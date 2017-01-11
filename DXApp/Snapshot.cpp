#include "stdafx.h"
#include "Snapshot.h"

namespace DXApp
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
		return WS_POPUP;
	}
	DWORD Snapshot::RetrieveExStyle()
	{
		return (WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
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
			dc.SelectObject(GetStockObject(NULL_BRUSH));
			::BitBlt(dc, 0, 0, TO_CX(s), TO_CY(s), m_hBDC, 0, 0, SRCCOPY);
			TinyRectangle rectangle = m_rectangle;
			rectangle.NormalizeRect();
			::BitBlt(dc, rectangle.left, rectangle.top, rectangle.Width(), rectangle.Height(), m_hFDC, rectangle.left, rectangle.top, SRCCOPY);
			this->Draw(&dc);
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

	LRESULT Snapshot::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(LOWORD(lParam), HIWORD(lParam));
		if (HitTest(point) >= 0)
		{
			Track(m_hWND, point, TRUE);
		}
		else
		{
			TrackRubberBand(m_hWND, point, TRUE);
		}
		Invalidate();
		return FALSE;
	}

	void Snapshot::OnChangedRect(const TinyRectangle& rectOld)
	{
		Invalidate();
	}

	LRESULT Snapshot::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow();
		}
		return FALSE;
	}

	LRESULT Snapshot::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_hWND, &point);
		if (this->SetCursor(m_hWND, LOWORD(lParam)))
		{
			bHandled = TRUE;
		}
		return FALSE;
	}

	LRESULT Snapshot::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}