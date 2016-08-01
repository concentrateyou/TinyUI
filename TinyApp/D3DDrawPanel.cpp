#include "stdafx.h"
#include "D3DDrawPanel.h"
#include "Render/TinyCanvas.h"

CD3DDrawPanel::CD3DDrawPanel()
{
}


CD3DDrawPanel::~CD3DDrawPanel()
{
}

BOOL CD3DDrawPanel::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}

LRESULT CD3DDrawPanel::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_hWND, &ps);
	TinyCanvas canvas(hDC);
	canvas.SetPen((HPEN)GetStockObject(BLACK_PEN));
	RECT s = ps.rcPaint;
	s.left += 1;
	s.top += 1;
	s.bottom -= 1;
	s.right -= 1;
	canvas.DrawRectangle(s);
	EndPaint(m_hWND, &ps);*/
	return FALSE;
}

LPCSTR CD3DDrawPanel::RetrieveClassName()
{
	return TEXT("CD3DDrawPanel");
}

LPCSTR CD3DDrawPanel::RetrieveTitle()
{
	return TEXT("CD3DDrawPanel");
}

HICON CD3DDrawPanel::RetrieveIcon()
{
	return NULL;
}

DWORD CD3DDrawPanel::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILDWINDOW);
}

DWORD CD3DDrawPanel::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_LTRREADING);
}
