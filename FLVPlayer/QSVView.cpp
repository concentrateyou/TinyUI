#include "stdafx.h"
#include "QSVView.h"

namespace FLVPlayer
{
	QSVView::QSVView()
	{
	}


	QSVView::~QSVView()
	{
	}

	LRESULT QSVView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}

	BOOL QSVView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD QSVView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD QSVView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR QSVView::RetrieveClassName()
	{
		return TEXT("QSVView");
	}

	LPCSTR QSVView::RetrieveTitle()
	{
		return TEXT("QSVView");
	}

	HICON QSVView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT QSVView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_task.Initialize(m_hWND))
		{
			m_task.Submit();
		}
		return FALSE;
	}

	LRESULT QSVView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_task.Close(3000);
		return FALSE;
	}

	LRESULT QSVView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT QSVView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}