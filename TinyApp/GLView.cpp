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
	m_graphics.GetRenderView()->BeginDraw();

	/*XMFLOAT2 points[4];
	points[0] = XMFLOAT2(-100.0f, 100.0f);
	points[1] = XMFLOAT2(100.0f, -100.0f);
	points[2] = XMFLOAT2(0.0f, -50.0f);
	points[3] = XMFLOAT2(-100.0f, 100.0f);
	m_line2D.Create(m_graphics.GetDX11(), points, 4, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));*/
	XMFLOAT2 points[4];
	points[0] = XMFLOAT2(10.0F, 10.0F);
	points[1] = XMFLOAT2(10.0F, 110.0F);
	points[2] = XMFLOAT2(110.0F, 110.0F);
	points[3] = XMFLOAT2(110.0F, 10.0F);
	/*points[0] = XMFLOAT2(-100.0F, 100.0F);
	points[1] = XMFLOAT2(100.0F, 100.0F);
	points[2] = XMFLOAT2(100.0F, -100.0F);
	points[3] = XMFLOAT2(-100.0F, -100.0F);*/
	m_rectangle2D.Create(m_graphics.GetDX11(), points, XMFLOAT4(1.0F, 0.0F, 0.0F, 1.0F));

	m_graphics.DrawRectangle(m_rectangle2D);

	m_graphics.GetRenderView()->EndDraw();
	m_graphics.Present();
	////TinyRectangle s;
	////GetClientRect(&s);
	////m_graphics.Initialize(m_hWND, s.Size());
	////m_graphics.SetRenderView(NULL);
	////m_image2D[0].Load(m_graphics.GetDX9(), "D:\\TransA.png");
	////m_image2D[0].SetScale(D3DXVECTOR2(static_cast<FLOAT>(TO_CX(s)) / m_image2D[0].GetSize().x, static_cast<FLOAT>(TO_CY(s)) / m_image2D[0].GetSize().y));
	////m_image2D[1].Load(m_graphics.GetDX9(), "D:\\1.png");
	////m_image2D[1].SetScale(D3DXVECTOR2(static_cast<FLOAT>(TO_CX(s)) / m_image2D[1].GetSize().x, static_cast<FLOAT>(TO_CY(s)) / m_image2D[1].GetSize().y));

	////m_graphics.GetRenderView()->BeginDraw();
	//////m_image2D[0].SetScale(D3DXVECTOR2(0.5F, 0.5F));
	//////m_image2D[0].SetTranslate(D3DXVECTOR2(50.0F, 50.0F));
	//////m_image2D[0].SetRotate(180.0F);
	////m_graphics.DrawImage(&m_image2D[0]);

	//////m_graphics.DrawImage(&m_image2D[1]);
	////m_graphics.GetRenderView()->EndDraw();
	////m_graphics.Present();
	//m_player.Open(m_hWND, "D:\\test.flv");
	/*LONGLONG s;
	m_player.GetDuration(s);
	m_player.SetPosition(s / 2);*/
	//m_player.Play();
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
	//m_graphics.Present();
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