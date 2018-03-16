#include "stdafx.h"
#include "HLSPreviewView.h"

namespace HLSPlayer
{
	HLSPreviewView::HLSPreviewView()
	{
	}


	HLSPreviewView::~HLSPreviewView()
	{
	}

	LRESULT HLSPreviewView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}

	BOOL HLSPreviewView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD HLSPreviewView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD HLSPreviewView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR HLSPreviewView::RetrieveClassName()
	{
		return TEXT("HLSPreviewView");
	}

	LPCSTR HLSPreviewView::RetrieveTitle()
	{
		return TEXT("HLSPreviewView");
	}

	HICON HLSPreviewView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT HLSPreviewView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT HLSPreviewView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT HLSPreviewView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT HLSPreviewView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}