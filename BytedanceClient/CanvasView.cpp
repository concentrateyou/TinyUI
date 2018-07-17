#include "stdafx.h"
#include "CanvasView.h"
#include "Render/TinyCanvas.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(CanvasView, TinyControl);

	CanvasView::CanvasView()
		:m_graphics(m_dx11)
	{
	}

	CanvasView::~CanvasView()
	{
	}

	LPCSTR CanvasView::RetrieveClassName()
	{
		return TEXT("CanvasView");
	}

	LPCSTR CanvasView::RetrieveTitle()
	{
		return TEXT("CanvasView");
	}

	DWORD CanvasView::RetrieveStyle()
	{
		return (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}

	DWORD CanvasView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING);
	}

	LRESULT CanvasView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyRectangle s;
		GetClientRect(&s);
		m_dx11.Initialize(m_hWND, TO_CX(s), TO_CY(s));
		m_image2D.Load(m_graphics.GetDX10(), "D:\\timg.jpg");
		m_rectangle2D.Create(m_dx11);
		m_graphics.Create();
		m_graphics.InitializeShaders();
		return TinyControl::OnCreate(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinySize size;
		size.cx = LOWORD(lParam);
		size.cy = HIWORD(lParam);
		m_graphics.Destory();
		m_dx11.Resize(size.cx, size.cy);
		m_graphics.Create();
		return TinyControl::OnSize(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s;
		BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);

		m_graphics.BeginDraw();

		/*XMFLOAT2 points[4];
		points[0] = { 1.0F,1.0F };
		points[1] = { 1.0F,301.0F };
		points[2] = { 301.0F,301.0F };
		points[3] = { 301.0F,1.0F };
		m_graphics.DrawRectangle(m_rectangle2D, points, { 0.0f, 1.0f, 0.0f, 1.0f });*/
		m_graphics.EndDraw(); 

		m_dx11.Present();

		return TinyControl::OnPaint(uMsg, wParam, lParam, bHandled);
	}
}