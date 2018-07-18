#include "stdafx.h"
#include "CanvasController.h"

namespace Bytedance
{
	CanvasController::CanvasController(CanvasView& view)
		:m_view(view),
		m_preview(m_dx10),
		m_graphics(m_dx10),
		m_bClose(FALSE)
	{
		m_videoEvent.CreateEvent();
	}

	CanvasController::~CanvasController()
	{
	}
	BOOL CanvasController::Initialize(const TinySize& size)
	{
		TinyRectangle s;
		GetClientRect(m_view.Handle(), &s);
		if (!m_dx10.Initialize(m_view.Handle(), TO_CX(s), TO_CY(s)))
			return FALSE;
		if (!m_preview.Create())
			return FALSE;
		if (!m_preview.InitializeShaders())
			return FALSE;
		if (!m_graphics.Create(size.cx, size.cy))
			return FALSE;
		if (!m_graphics.InitializeShaders())
			return FALSE;
		m_bClose = FALSE;
		m_task.Submit(BindCallback(&CanvasController::OnMessagePump, this));
		return TRUE;
	}
	void CanvasController::Uninitialize()
	{
		m_bClose = TRUE;
		m_task.Close(3000);
		m_preview.Destory();
		m_graphics.Destory();
	}
	void CanvasController::OnMessagePump()
	{
		for (;;)
		{
			if (m_videoEvent.Lock())
			{
				if (m_bClose)
					break;
				PreviewRender();
				EncodeRender();
			}
		}
	}
	void CanvasController::PreviewRender()
	{
		TinyAutoLock lock(m_lock);
		m_preview.BeginDraw();
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual* visual = m_visuals[i];
			visual->Process();
			m_graphics.DrawImage(*visual->visual());
		}
		m_preview.EndDraw();
	}
	void CanvasController::EncodeRender()
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
