#include "stdafx.h"
#include "CanvasController.h"

namespace MediaSDK
{
	CanvasController::CanvasController(CanvasView& view)
		:m_view(view),
		m_display(m_dx11),
		m_video(m_dx11)
	{

	}

	CanvasController::~CanvasController()
	{

	}

	BOOL CanvasController::Initialize(const TinySize& size)
	{
		return TRUE;
	}

	void CanvasController::Add(IVisual2D* visual)
	{
		TinyAutoLock lock(m_lock);
		m_visuals.Add(visual);
	}
	void CanvasController::Remove(IVisual2D* visual)
	{
		TinyAutoLock lock(m_lock);
		m_visuals.Remove(visual);
	}
	void CanvasController::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		m_visuals.RemoveAll();
	}
}
