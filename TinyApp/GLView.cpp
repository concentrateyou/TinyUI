#include "stdafx.h"
#include "GLView.h"


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
	m_graphics.Initialize(m_hWND, s.Size());
	m_graphics.SetRenderView(NULL);
	m_image2D[0].Load(m_graphics.GetDX9(), "D:\\TransA.png");
	m_image2D[0].SetRectangle(s);
	m_image2D[1].Load(m_graphics.GetDX9(), "D:\\1.png");
	m_image2D[1].SetRectangle(s);

	m_graphics.GetRenderView()->BeginDraw();
	m_graphics.DrawImage(&m_image2D[0]);
	m_graphics.DrawImage(&m_image2D[1]);
	m_graphics.GetRenderView()->EndDraw();
	m_graphics.Present();
	/*m_player.Open(m_hWND, "D:\\Ñ¸À×ÏÂÔØ\\Ñª¹ÛÒô\\[Ñª¹ÛÒô]The.Bold.the.Corrupt.and.the.Beautiful.2017.BluRay.720p.x264.AC3-CnSCG.mkv");
	LONGLONG s;
	m_player.GetDuration(s);
	m_player.SetPosition(s / 2);
	m_player.Play();*/
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
	m_graphics.Present();
	return FALSE;
}

LRESULT GLView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
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