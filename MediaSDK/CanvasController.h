#pragma once
#include "CanvasView.h"
#include "CameraVisual2D.h"
#include "GameVisual2D.h"
#include "MonitorVisual2D.h"
#include "StreamVisual2D.h"

namespace MediaSDK
{
	class CanvasController
	{
		DISALLOW_COPY_AND_ASSIGN(CanvasController)
	public:
		CanvasController(CanvasView& view);
		~CanvasController();
		BOOL   Initialize(const TinySize& size);
	public:
		void	Add(IVisual2D* visual);
		void	Remove(IVisual2D* visual);
		void	RemoveAll();
	private:
		void	OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
		void	OnLButtonUp(UINT, WPARAM, LPARAM, BOOL&);
		void	OnMouseMove(UINT, WPARAM, LPARAM, BOOL&);
		void	OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
		void	OnSize(UINT, WPARAM, LPARAM, BOOL&);
	private:
		DX11					m_dx11;
		DX11Graphics2D			m_display;
		DX11Graphics2D			m_video;
		TinyLock				m_lock;
		IVisual2D*				m_current;
		TinyArray<IVisual2D*>	m_visuals;
		CanvasView&				m_view;
	private:
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onMouseLeave;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onSize;
	};
}

