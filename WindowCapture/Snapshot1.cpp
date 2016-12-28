#include "stdafx.h"
#include "Snapshot1.h"

namespace WindowCapture
{
	Snapshot1::Snapshot1()
	{

	}
	Snapshot1::~Snapshot1()
	{
	}
	BOOL Snapshot1::Create(HWND hParent)
	{
		return TinyControl::Create(hParent, 100, 100, 600, 600, FALSE);
	}
	DWORD Snapshot1::RetrieveStyle()
	{
		return WS_THICKFRAME;
	}
	DWORD Snapshot1::RetrieveExStyle()
	{
		return (WS_EX_TOOLWINDOW);
	}
	LPCSTR Snapshot1::RetrieveClassName()
	{
		return TEXT("Snapshot");
	}
	LPCSTR Snapshot1::RetrieveTitle()
	{
		return TEXT("Snapshot");
	}
	HICON Snapshot1::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT Snapshot1::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s = { 10,10,100,100 };
		m_tracker = new Tracker(&s,
			Tracker::hatchInside |
			Tracker::hatchedBorder |
			Tracker::resizeInside
		);
		return FALSE;
	}
	LRESULT Snapshot1::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		SAFE_DELETE(m_tracker);
		return FALSE;
	}
	LRESULT Snapshot1::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
	LRESULT Snapshot1::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m_hWND, &ps);
		m_tracker->Draw(hDC);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}
	LRESULT Snapshot1::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT Snapshot1::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_tracker->SetCursor(m_hWND, HTCLIENT);
		return FALSE;
	}

	LRESULT Snapshot1::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		POINT pos = { LOWORD(lParam) ,HIWORD(lParam) };
		if (m_tracker->Track(m_hWND, pos, FALSE, GetParent(m_hWND)))
		{
			/*RECT aRect = m_tracker->m_rect;
			TransformRect(&aRect, m_hWND, GetParent(m_hWND));*/
		}
		return FALSE;
	}

	LRESULT Snapshot1::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_tracker->SetCursor(m_hWND, HTCLIENT);
		return FALSE;
	}

	LRESULT Snapshot1::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}