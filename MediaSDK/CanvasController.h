#pragma once
#include "CanvasView.h"
#include "VideoWorker.h"

namespace MediaSDK
{
	class CanvasController
	{
		DISALLOW_COPY_AND_ASSIGN(CanvasController)
	public:
		CanvasController(CanvasView& view, TinyScopedReferencePtr<TinyTaskRunner> runner);
		~CanvasController();
		BOOL			Initialize(const TinySize& size);
		VideoWorker&	GetVideoWorker();
	private:
		void			OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
		void			OnLButtonUp(UINT, WPARAM, LPARAM, BOOL&);
		void			OnMouseMove(UINT, WPARAM, LPARAM, BOOL&);
		void			OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
		void			OnSize(UINT, WPARAM, LPARAM, BOOL&);
		void			OnSetCursor(UINT, WPARAM, LPARAM, BOOL&);
	private:
		CanvasView&					m_view;
		IVisual2D*					m_current;
		TinyArray<IVisual2D*>		m_visuals;
		VideoWorker					m_videoWorker;
	};
}

