#include "stdafx.h"
#include "CanvasView.h"
#include "Render/TinyCanvas.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(CanvasView, TinyControl);

	CanvasView::CanvasView()
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
		return TinyControl::OnCreate(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinySize size;
		size.cx = LOWORD(lParam);
		size.cy = HIWORD(lParam);
		return TinyControl::OnSize(uMsg, wParam, lParam, bHandled);
	}

	LRESULT CanvasView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return TinyControl::OnPaint(uMsg, wParam, lParam, bHandled);
	}
}