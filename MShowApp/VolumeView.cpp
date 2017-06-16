#include "stdafx.h"
#include "VolumeView.h"
#include "Resource.h"

namespace MShow
{
	VolumeView::VolumeView()
		:m_bMute(FALSE)
	{
	}

	VolumeView::~VolumeView()
	{
	}
	LRESULT VolumeView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL VolumeView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD VolumeView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD VolumeView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR VolumeView::RetrieveClassName()
	{
		return TEXT("VolumeView");
	}

	LPCSTR VolumeView::RetrieveTitle()
	{
		return TEXT("VolumeView");
	}

	HICON VolumeView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT VolumeView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_hICONS[0] = LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_ICON1));
		m_hICONS[1] = LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_ICON2));
		return FALSE;
	}

	LRESULT VolumeView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VolumeView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawIcon(hDC, 0, 0, m_hICONS[m_bMute]);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT VolumeView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VolumeView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VolumeView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_bMute = !m_bMute;
		return FALSE;
	}

}


