#pragma once
#include "IO/TinyWin32Threadpool.h"
#include "DXFramework.h"
#include "DX10.h"
#include "DX10Graphics2D.h"
#include "DX10Image2D.h"
#include "CanvasView.h"
#include "GameVisual.h"
#include "CameraVisual.h"
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
		BOOL Initialize(const TinySize& size);
		void Uninitialize();
	public:
		void Add(IVisual* ps);
		void Remove(IVisual* ps);
	private:
		void OnMessagePump();
		void PreviewRender();
		void EncodeRender();
	private:
		BOOL					m_bClose;
		TinyLock				m_lock;
		TinyEvent				m_videoEvent;
		DX10					m_dx10;
		DX10Graphics2D			m_preview;//‘§¿¿
		DX10Graphics2D			m_graphics;//±‡¬Î
		TinyArray<IVisual*>		m_visuals;
		TinyThread				m_task;
		TinyPerformanceTime		m_time;
		TinyPerformanceTimer	m_timer;
		CanvasView&				m_view;
	};
}

