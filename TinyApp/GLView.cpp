#include "stdafx.h"
#include "GLView.h"


HHOOK GLView::g_hhk = NULL;

GLView::GLView()
{
}


GLView::~GLView()
{
}

LRESULT GLView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

DWORD GLView::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD GLView::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR GLView::RetrieveClassName()
{
	return TEXT("GLView");
}

LPCSTR GLView::RetrieveTitle()
{
	return TEXT("GLView");
}

HICON GLView::RetrieveIcon()
{
	return NULL;
}

BOOL GLView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}

LRESULT GLView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	TinyRectangle s;
	GetClientRect(&s);
	m_graphics.Initialize(m_hWND, s.Width(), s.Height());
	m_rectangle2D.Create(m_graphics.GetGL());
	//m_image.Load(m_graphics.GetGL(), "D:\\timg.jpg");
	m_graphics.BeginDraw();
	XMFLOAT2 points[4];
	points[0] = XMFLOAT2(100.0F, 100.0F);
	points[1] = XMFLOAT2(200.0F, 100.0F);
	points[2] = XMFLOAT2(200.0F, 200.0F);
	points[3] = XMFLOAT2(100.0F, 200.0F);
	m_graphics.DrawRectangle(m_rectangle2D, points, XMFLOAT4(0.5F, 0.0F, 0.0F, 1.0F));
	m_graphics.EndDraw();

	return FALSE;
}

LRESULT GLView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_graphics.Resize(LOWORD(lParam), HIWORD(lParam));
	m_graphics.BeginDraw();
	XMFLOAT2 points[4];
	points[0] = XMFLOAT2(100.0F, 100.0F);
	points[1] = XMFLOAT2(200.0F, 100.0F);
	points[2] = XMFLOAT2(200.0F, 200.0F);
	points[3] = XMFLOAT2(100.0F, 200.0F);
	m_graphics.DrawRectangle(m_rectangle2D, points, XMFLOAT4(0.5F, 0.0F, 0.0F, 1.0F));
	m_graphics.EndDraw();
	return FALSE;
}

LRESULT GLView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//m_player.Close();
	return FALSE;
}

LRESULT GLView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_graphics.BeginDraw();
	XMFLOAT2 points[4];
	points[0] = XMFLOAT2(100.0F, 100.0F);
	points[1] = XMFLOAT2(200.0F, 100.0F);
	points[2] = XMFLOAT2(200.0F, 200.0F);
	points[3] = XMFLOAT2(100.0F, 200.0F);
	m_graphics.DrawRectangle(m_rectangle2D, points, XMFLOAT4(0.5F, 0.0F, 0.0F, 1.0F));
	m_graphics.EndDraw();
	return FALSE;
}

LRESULT GLView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return FALSE;
}

LRESULT GLView::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT GLView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

LRESULT GLView::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

#define KEYDOWN(code) ((GetAsyncKeyState(code) & 0x8000) ? 1 : 0) 
#define KEYUP(code) ((GetAsyncKeyState(code) & 0x8000) ? 0 : 1) 

void GLView::OnTimer()
{

	/*HWND hWND = (HWND)0x023E0230;
	SetWindowPos(hWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
	RECT s;
	::GetWindowRect(hWND, &s);
	POINT pos;
	::GetCursorPos(&pos);
	if (::PtInRect(&s, pos))
	{
		HWND hWND_1 = (HWND)0x01E401FA;
		::ScreenToClient(hWND_1, &pos);
		if (KEYDOWN(VK_LBUTTON))
		{
			::PostMessage(hWND_1, WM_LBUTTONDOWN, NULL, MAKELPARAM(pos.x, pos.y));
			::PostMessage(hWND_1, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(pos.x, pos.y));
		}
		else if (KEYUP(VK_LBUTTON))
		{
			::PostMessage(hWND_1, WM_LBUTTONUP, NULL, MAKELPARAM(pos.x, pos.y));
			::PostMessage(hWND_1, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(pos.x, pos.y));
		}
		else
		{
			::PostMessage(hWND_1, WM_MOUSEMOVE, NULL, MAKELPARAM(pos.x, pos.y));
		}
	}*/
}
LRESULT CALLBACK GLView::MouseFilterHook(INT code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		MOUSEHOOKSTRUCT * ms = (MOUSEHOOKSTRUCT *)lParam;
		if (ms != NULL)
		{
			if (wParam == WM_LBUTTONDOWN)
			{

			}
		}
	}
	return CallNextHookEx(g_hhk, code, wParam, lParam);
}