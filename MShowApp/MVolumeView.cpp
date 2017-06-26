#include "stdafx.h"
#include "MVolumeView.h"
#include "resource.h"

namespace MShow
{
	MVolumeView::MVolumeView()
		:m_bFlag(TRUE)
	{
	}

	MVolumeView::~MVolumeView()
	{
	}
	LRESULT MVolumeView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL MVolumeView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MVolumeView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MVolumeView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MVolumeView::RetrieveClassName()
	{
		return TEXT("MVolumeView");
	}

	LPCSTR MVolumeView::RetrieveTitle()
	{
		return TEXT("MVolumeView");
	}

	HICON MVolumeView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MVolumeView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_hICONS[0] = LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_ICON2));
		m_hICONS[1] = LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_ICON1));
		m_onPosChange.Reset(new Delegate<void(void*, INT)>(this, &MVolumeView::OnPosChange));
		m_trackBar.Create(m_hWND, 25, 0, 150, 20);
		m_trackBar.SetRange(0, 100);
		m_trackBar.EVENT_POSCHANGING += m_onPosChange;
		return FALSE;
	}

	LRESULT MVolumeView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_trackBar.EVENT_POSCHANGING -= m_onPosChange;
		return FALSE;
	}

	LRESULT MVolumeView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
		::DrawIconEx(hDC, 0, 0, m_hICONS[m_bFlag], 20, 20, 0, NULL, DI_NORMAL);
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MVolumeView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MVolumeView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}

	LRESULT MVolumeView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_bFlag = !m_bFlag;
		m_trackBar.SetPos(m_bFlag ? 0 : 100);
		this->Invalidate();
		return FALSE;
	}

	void MVolumeView::OnPosChange(void*, INT pos)
	{
		m_bFlag = (pos == 0 ? TRUE : FALSE);
		this->Invalidate();
		EVENT_VOLUME(pos);
	}
}