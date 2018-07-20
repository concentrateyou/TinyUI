#include "stdafx.h"
#include "CanvasController.h"

namespace Bytedance
{
	CanvasController::CanvasController(CanvasView& view)
		:m_view(view),
		m_displayGS(m_dx11),
		m_videoGS(m_dx11),
		m_bClose(FALSE)
	{
		m_videoEvent.CreateEvent();
	}

	CanvasController::~CanvasController()
	{
	}
	DX11& CanvasController::GetDX11()
	{
		return m_dx11;
	}
	BOOL CanvasController::Initialize(const TinySize& size)
	{
		TinyRectangle s;
		GetClientRect(m_view.Handle(), &s);
		if (!m_dx11.Initialize(m_view.Handle(), TO_CX(s), TO_CY(s)))
			return FALSE;
		if (!m_displayGS.Create())
			return FALSE;
		if (!m_displayGS.InitializeShaders())
			return FALSE;
		if (!m_videoGS.Create(size.cx, size.cy))
			return FALSE;
		if (!m_videoGS.InitializeShaders())
			return FALSE;
		m_bClose = FALSE;
		m_task.Submit(BindCallback(&CanvasController::OnMessagePump, this));
		return TRUE;
	}
	void CanvasController::Uninitialize()
	{
		m_bClose = TRUE;
		m_task.Close(3000);
		m_displayGS.Destory();
		m_videoGS.Destory();
	}
	void CanvasController::OnMessagePump()
	{
		for (;;)
		{
			DisplayRender();
			VideoRender();
		}
	}
	void CanvasController::DisplayRender()
	{
		TinyAutoLock lock(m_lock);
		m_displayGS.BeginDraw();
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual* visual = m_visuals[i];
			if (visual->Process())
			{
				if (visual->visual()->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
				{
					m_videoGS.DrawImage(*static_cast<DX11Image2D*>(visual->visual()));
				}
				if (visual->visual()->IsKindOf(RUNTIME_CLASS(DX11YUVVideo)))
				{
					m_videoGS.DrawImageYUVBT601(*static_cast<DX11YUVVideo*>(visual->visual()));
				}
				if (visual->visual()->IsKindOf(RUNTIME_CLASS(DX11YUY2Video)))
				{
					m_videoGS.DrawImageYUY2BT709(*static_cast<DX11YUY2Video*>(visual->visual()));
				}
			}
		}
		m_displayGS.EndDraw();
		m_dx11.Present();
	}
	void CanvasController::VideoRender()
	{

	}
	void CanvasController::Add(IVisual* ps)
	{
		TinyAutoLock lock(m_lock);
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual* visual = m_visuals[i];
			if (strcasecmp(visual->Name(), ps->Name()) == 0)
			{
				m_visuals.Remove(visual);
				SAFE_DELETE(visual);
				break;
			}
		}
		m_visuals.Add(ps);
	}
	void CanvasController::Remove(IVisual* ps)
	{
		TinyAutoLock lock(m_lock);
		m_visuals.Remove(ps);
	}
}
