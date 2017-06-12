#include "stdafx.h"
#include "DXView.h"

namespace MShow
{
	DXView::DXView()
	{
	}

	DXView::~DXView()
	{
	}
	LRESULT DXView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL DXView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD DXView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD DXView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR DXView::RetrieveClassName()
	{
		return TEXT("DXView");
	}

	LPCSTR DXView::RetrieveTitle()
	{
		return TEXT("DXView");
	}

	HICON DXView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT DXView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_controller.Reset(new MShowController(*this));
		if (m_controller)
		{
			m_controller->Initialize("rtmp://10.10.13.98/live/lb_kaixinjuchang_720p");
		}
		return FALSE;
	}

	LRESULT DXView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_controller)
		{
			m_controller->Uninitialize();
		}
		return FALSE;
	}

	LRESULT DXView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT DXView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT DXView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}


