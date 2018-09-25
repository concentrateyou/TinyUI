#include "stdafx.h"
#include "CanvasController.h"

namespace MediaSDK
{
	CanvasController::CanvasController(CanvasView& view, TinyScopedReferencePtr<TinyTaskRunner> runner)
		:m_view(view),
		m_current(NULL),
		m_videoWorker(runner)
	{

	}

	CanvasController::~CanvasController()
	{

	}

	BOOL CanvasController::Initialize(const TinySize& size)
	{
		m_view.EVENT_SIZE += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnSize);
		m_view.EVENT_LBUTTONDOWN += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnLButtonDown);
		m_view.EVENT_LBUTTONUP += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnLButtonUp);
		m_view.EVENT_MOUSEMOVE += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnMouseMove);
		m_view.EVENT_MOUSELEAVE += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnMouseLeave);
		m_view.EVENT_SETCURSOR += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnSetCursor);
		if (!m_videoWorker.Initialize(m_view.Handle(), size, { 1280,720 }))
			return FALSE;
		m_videoWorker.Start();
		return TRUE;
	}

	VideoWorker& CanvasController::GetVideoWorker()
	{
		return m_videoWorker;
	}

	void CanvasController::OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		IVisual2D* hittest = m_videoWorker.HitTest(point);
		if (hittest != m_current)
		{
			m_current = hittest;
		}
		if (m_current != NULL && m_current->HitTest(point) >= 0)
		{
			m_current->Track(m_view.Handle(), point, FALSE);
		}
	}

	void CanvasController::OnLButtonUp(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		TinyPoint pos(LOWORD(lParam), HIWORD(lParam));

	}

	void CanvasController::OnMouseMove(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		TinyPoint pos(LOWORD(lParam), HIWORD(lParam));
	}

	void CanvasController::OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&)
	{

	}

	void CanvasController::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&)
	{
		TinySize size(LOWORD(lParam), HIWORD(lParam));
		m_videoWorker.Resize(size);
	}

	void CanvasController::OnSetCursor(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_view.Handle(), &point);
		if (m_current = m_videoWorker.HitTest(point))
		{
			if (m_current->SetCursor(m_view.Handle(), LOWORD(lParam)))
			{
				bHandled = TRUE;
			}
		}
	}
}
