#pragma once
#include "IO/TinyWin32Threadpool.h"
#include "DXFramework.h"
#include "DX11.h"
#include "DX11Graphics2D.h"
#include "DX11Image2D.h"
#include "CanvasView.h"
#include "GameElement.h"
#include "CameraElement.h"
using namespace DXFramework;
using namespace TinyFramework;

namespace Bytedance
{
	class CanvasController
	{
		DISALLOW_COPY_AND_ASSIGN(CanvasController)
	public:
		CanvasController(CanvasView& view);
		~CanvasController();
		DX11& GetDX11();
		BOOL Initialize(const TinySize& output);
		void Uninitialize();
	public:
		void Add(IVisual* ps);
		void Remove(IVisual* ps);
	private:
		void OnMessagePump();
		void DisplayRender();
		void VideoRender();
	private:
		BOOL					m_bClose;
		TinyLock				m_lock;
		DX11					m_dx11;
		DX11Graphics2D			m_displayCanvas;//‘§¿¿
		DX11Graphics2D			m_videoCanvas;//±‡¬Î
		TinyArray<IVisual*>		m_visuals;
		TinyWin32Threadpool		m_tasks;
		TinyScopedPtr<TinyWin32Worker>	m_woker;
		TinyPerformanceTime		m_time;
		TinyPerformanceTimer	m_timer;
		CanvasView&				m_view;
	};
}

