#pragma once
#include "CanvasView.h"
#include "CameraVisual2D.h"
#include "GameVisual2D.h"
#include "MonitorVisual2D.h"
#include "StreamVisual2D.h"
#include "ImageVisual2D.h"

namespace MediaSDK
{
	class CanvasController
	{
		DISALLOW_COPY_AND_ASSIGN(CanvasController)
	public:
		CanvasController(CanvasView& view);
		~CanvasController();
		DX11&		GetDX11();
		BOOL		Initialize(const TinySize& size);
	public:
		void		Add(IVisual2D* visual);
		void		Remove(IVisual2D* visual);
		void		RemoveAll();
		void		MoveUp(IVisual2D* visual);
		void		MoveDown(IVisual2D* visual);
	private:
		void		OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
		void		OnLButtonUp(UINT, WPARAM, LPARAM, BOOL&);
		void		OnMouseMove(UINT, WPARAM, LPARAM, BOOL&);
		void		OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
		void		OnSize(UINT, WPARAM, LPARAM, BOOL&);
		void		OnSetCursor(UINT, WPARAM, LPARAM, BOOL&);
		void		OnDraw();
		void		OnRender();
		IVisual2D*	HitTest(const TinyPoint& pos);
	private:
		DX11						m_dx11;
		DX11Graphics2D				m_display;
		DX11Graphics2D				m_video;
		CanvasView&					m_view;
		TinyLock					m_lock;
		IVisual2D*					m_current;
		TinyArray<IVisual2D*>		m_visuals;
		TinyWorkerPoolTaskRunner	m_works;
	};
}

