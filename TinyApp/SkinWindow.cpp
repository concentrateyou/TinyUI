#include "stdafx.h"
#include "SkinWindow.h"


SkinWindow::SkinWindow()
{
}


SkinWindow::~SkinWindow()
{
}
LRESULT SkinWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL SkinWindow::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD SkinWindow::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW & ~WS_CAPTION);
}

DWORD SkinWindow::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR SkinWindow::RetrieveClassName()
{
	return TEXT("SkinWindow");
}

LPCSTR SkinWindow::RetrieveTitle()
{
	return TEXT("SkinWindow");
}

HICON SkinWindow::RetrieveIcon()
{
	return NULL;
}

LRESULT SkinWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_imageBKG.Open("D:\\skin\\main_bkg.png");
	return FALSE;
}

LRESULT SkinWindow::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT SkinWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT s = { 0 };
	HDC hDC = BeginPaint(m_hWND, &s);
	OnDraw(hDC, s.rcPaint);
	EndPaint(m_hWND, &s);
	return FALSE;
}

LRESULT SkinWindow::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return TRUE;
}
LRESULT SkinWindow::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (static_cast<BOOL>(wParam))
	{
		NCCALCSIZE_PARAMS* ps = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
		ps->rgrc[0].left = ps->lppos->x < 0 ? 0 : ps->lppos->x;
		ps->rgrc[0].top = ps->lppos->y < 0 ? 0 : ps->lppos->y;
		ps->rgrc[0].bottom = ps->rgrc[0].top + ps->lppos->cy;
		ps->rgrc[0].right = ps->rgrc[0].left + ps->lppos->cx;
	}
	return TRUE;
}
LRESULT SkinWindow::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	TinyPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	TinyRectangle rectangle;
	this->GetWindowRect(rectangle);
	if (!rectangle.PtInRect(pos))
		return HTNOWHERE;
	INT cx = GetSystemMetrics(SM_CXBORDER);
	INT cy = GetSystemMetrics(SM_CYBORDER);
	if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
		return HTTOPLEFT;
	if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y > (rectangle.top + cy) && pos.y < (rectangle.bottom - cy))
		return HTLEFT;
	if (pos.x >= rectangle.left && pos.x <= (rectangle.left + cx) && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
		return HTBOTTOMLEFT;
	if (pos.x > (rectangle.left + cx) && pos.x < (rectangle.right - cx) && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
		return HTTOP;
	if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y >= rectangle.top && pos.y <= (rectangle.top + cy))
		return HTTOPRIGHT;
	if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y > (rectangle.top + cy) && pos.y < (rectangle.bottom - cy))
		return HTRIGHT;
	if (pos.x >= (rectangle.right - cx) && pos.x <= rectangle.right && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
		return HTBOTTOMRIGHT;
	if (pos.x > (rectangle.left + cx) && pos.x < (rectangle.right - cx) && pos.y >= (rectangle.bottom - cy) && pos.y <= rectangle.bottom)
		return HTBOTTOM;
	::ScreenToClient(m_hWND, &pos);
	return HTCLIENT;
}

void SkinWindow::OnDraw(HDC hDC, RECT& rectangle)
{
	TinyMemDC dc(hDC, m_imageBKG);
	TinyRectangle rect = m_imageBKG.GetRectangle();
	dc.Render(rectangle, rect, TRUE);
}
