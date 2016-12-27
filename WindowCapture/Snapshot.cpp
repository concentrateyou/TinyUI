#include "stdafx.h"
#include "Snapshot.h"

namespace WindowCapture
{
	Snapshot::Snapshot()
	{
	}
	Snapshot::~Snapshot()
	{
	}
	BOOL Snapshot::Create(HWND hParent)
	{
		HWND hDesktop = ::GetDesktopWindow();
		TinyRectangle s;
		::GetWindowRect(hDesktop, &s);
		HDC  hDesktopDC = ::GetDC(hDesktop);
		ReleaseDC(hDesktop, hDesktopDC);
		m_backgroundDC.CreateCompatibleDC(hDesktopDC);
		m_backgroundBitmap.CreateDiscardableBitmap(hDesktopDC, s.Width(), s.Height());
		m_backgroundDC.SelectObject(m_backgroundBitmap);
		m_foregroundDC.CreateCompatibleDC(hDesktopDC);
		m_foregroundBitmap.CreateDiscardableBitmap(hDesktopDC, s.Width(), s.Height());
		m_foregroundDC.SelectObject(m_foregroundBitmap);
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

	LRESULT Snapshot::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT Snapshot::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
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
		return FALSE;
	}
	LRESULT Snapshot::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}

	LRESULT Snapshot::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	LRESULT Snapshot::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	LRESULT Snapshot::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	LRESULT Snapshot::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}