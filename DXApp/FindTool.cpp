#include "stdafx.h"
#include "FindTool.h"


FindTool::FindTool()
	:m_bDragging(FALSE),
	m_hCursor(NULL)
{
}


FindTool::~FindTool()
{
}

LRESULT FindTool::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
}

LRESULT FindTool::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetCapture(m_hWND);
	TinyPoint point(LOWORD(lParam), HIWORD(lParam));
	m_bDragging = TRUE;
	return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
}

LRESULT FindTool::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ReleaseCapture();
	m_bDragging = FALSE;
	return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
}

LRESULT FindTool::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return TinyLabel::OnLButtonDBClick(uMsg, wParam, lParam, bHandled);
}

LRESULT FindTool::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bitmapDrag1.LoadBitmap(hInstance, IDB_BITMAP1);
	m_bitmapDrag2.LoadBitmap(hInstance, IDB_BITMAP2);
	m_hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	SetBitmap(m_bitmapDrag1.Handle());
	Invalidate();
	return TinyLabel::OnCreate(uMsg, wParam, lParam, bHandled);
}

LRESULT FindTool::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hCursor)
	{
		DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	}
	return TinyLabel::OnDestory(uMsg, wParam, lParam, bHandled);
}

BOOL FindTool::BeginDrag()
{

}


