#include "stdafx.h"
#include "CanvasController.h"

namespace MediaSDK
{
	CanvasController::CanvasController(CanvasView& view)
		:m_view(view),
		m_display(m_dx11),
		m_video(m_dx11),
		m_current(NULL)
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
		return TRUE;
	}

	void CanvasController::Add(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.Add(visual);
	}

	void CanvasController::Remove(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.Remove(visual);
	}

	void CanvasController::RemoveAll()
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.RemoveAll();
	}

	void CanvasController::OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		TinyPoint pos(LOWORD(lParam), HIWORD(lParam));

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

	void CanvasController::OnSize(UINT, WPARAM, LPARAM, BOOL&)
	{

	}
}
