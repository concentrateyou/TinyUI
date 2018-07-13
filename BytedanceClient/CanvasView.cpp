#include "stdafx.h"
#include "CanvasView.h"
#include "Render/TinyCanvas.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(CanvasView, TinyControl);

	CanvasView::CanvasView()
		:m_renderView(m_graphics.GetDX9())
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
		m_graphics.Initialize(m_hWND, s.Size());
		/*m_renderView.Create();
		m_graphics.SetRenderView(&m_renderView);
		m_image2D.Load(m_graphics.GetDX9(), "D:\\timg.jpg");*/
		return TinyControl::OnCreate(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinySize size;
		size.cx = LOWORD(lParam);
		size.cy = HIWORD(lParam);
		m_graphics.Resize(size);
		//m_renderView.Resize();
		return TinyControl::OnSize(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s;
		BeginPaint(m_hWND, &s);
		EndPaint(m_hWND, &s);

		//m_graphics.GetRenderView()->BeginDraw();
		//m_graphics.DrawImage(&m_image2D);
		//m_graphics.GetRenderView()->EndDraw();
		//m_graphics.Present();

		return TinyControl::OnPaint(uMsg, wParam, lParam, bHandled);
	}
}