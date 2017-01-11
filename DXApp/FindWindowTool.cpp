#include "stdafx.h"
#include "FindWindowTool.h"
#include "Resource.h"

namespace DXApp
{
	FindWindowTool::FindWindowTool()
		:m_bDragging(FALSE),
		m_hCursor(NULL),
		m_hOldCursor(NULL),
		m_lastHWND(NULL),
		m_bDisplay(FALSE)
	{
	}


	FindWindowTool::~FindWindowTool()
	{
	}

	LRESULT FindWindowTool::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TinyPoint pos(LOWORD(lParam), HIWORD(lParam));
		if (m_bDragging && m_lastPos != pos)
		{
			m_lastPos = pos;
			::ClientToScreen(m_hWND, (POINT *)&pos);
			HWND hWND = WindowFromPoint(pos);
			if (hWND && hWND != m_lastHWND)
			{
				InvertWindow(m_lastHWND);
				InvertWindow(hWND);
				m_lastHWND = hWND;
				EVENT_WINDOWCHANGE(m_lastHWND);
			}
		}
		return TinyLabel::OnMouseMove(uMsg, wParam, lParam, bHandled);
	}

	LRESULT FindWindowTool::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetCapture(m_hWND);
		m_lastPos.x = LOWORD(lParam);
		m_lastPos.y = HIWORD(lParam);
		SetBitmap(m_bitmapDrag2.Handle());
		m_lastHWND = m_hWND;
		InvertWindow(m_lastHWND);
		m_hOldCursor = ::SetCursor(m_hCursor);
		m_bDragging = TRUE;
		if (!m_bDisplay)
		{
			HWND hWND = GetParent(GetParent(m_hWND));
			::ShowWindow(hWND, SW_HIDE);
		}
		return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
	}

	LRESULT FindWindowTool::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ReleaseCapture();
		SetBitmap(m_bitmapDrag1.Handle());
		SetCursor(m_hOldCursor);
		m_bDragging = FALSE;
		InvertWindow(m_lastHWND);
		if (!m_bDisplay)
		{
			HWND hWND = GetParent(GetParent(m_hWND));
			::ShowWindow(hWND, SW_SHOW);
		}
		return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
	}

	LRESULT FindWindowTool::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
	}

	LRESULT FindWindowTool::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TinyLabel::OnCreate(uMsg, wParam, lParam, bHandled);
	}

	LRESULT FindWindowTool::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_hCursor)
		{
			DestroyCursor(m_hCursor);
			m_hCursor = NULL;
		}
		return TinyLabel::OnDestory(uMsg, wParam, lParam, bHandled);
	}

	void FindWindowTool::LoadResource()
	{
		ModifyStyle(SS_RIGHT | SS_CENTER | SS_CENTERIMAGE | SS_ICON | SS_SIMPLE | SS_LEFTNOWORDWRAP, SS_NOTIFY | SS_BITMAP);
		m_bitmapDrag1.LoadBitmap(GetModuleHandle(NULL), IDB_BITMAP1);
		m_bitmapDrag2.LoadBitmap(GetModuleHandle(NULL), IDB_BITMAP2);
		m_hCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_CURSOR1));
		SetBitmap(m_bitmapDrag1.Handle());
	}

	void FindWindowTool::ShowTool(BOOL bShow)
	{
		m_bDisplay = bShow;
	}

	LRESULT FindWindowTool::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		if (m_bDragging)
		{
			bHandled = TRUE;
			SetCursor(m_hCursor);
		}
		return FALSE;
	}
}