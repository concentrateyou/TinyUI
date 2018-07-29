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
		DX11					m_dx11;
		DX11Graphics2D			m_display;
		DX11Graphics2D			m_video;
		CanvasView&				m_view;
		TinyWin32Threadpool		m_pool;
		TinyLock				m_lock;
		TinyArray<IVisual2D*>	m_visuals;
	};
}

