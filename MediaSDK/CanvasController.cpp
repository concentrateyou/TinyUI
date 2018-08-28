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
		m_view.EVENT_SETCURSOR += Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &CanvasController::OnSetCursor);
		if (!m_dx11.Initialize(m_view.Handle(), size.cx, size.cy))
			return FALSE;
		if (!m_display.InitializeShaders())
			return FALSE;
		if (!m_display.Create())
			return FALSE;
		if (!m_video.InitializeShaders())
			return FALSE;
		if (!m_video.Create(1280, 720))
			return FALSE;
		m_works.PostTask(BindCallback(&CanvasController::OnDraw, this), 0);
		return TRUE;
	}

	DX11& CanvasController::GetDX11()
	{
		return m_dx11;
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

	void CanvasController::MoveUp(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		INT index = m_visuals.Lookup(visual);
		if (index > 0)
		{
			IVisual2D* val = m_visuals[index - 1];
			m_visuals[index - 1] = visual;
			m_visuals[index] = val;
		}
	}

	void CanvasController::MoveDown(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		INT index = m_visuals.Lookup(visual);
		if (index > 0 && index < m_visuals.GetSize())
		{
			IVisual2D* val = m_visuals[index + 1];
			m_visuals[index + 1] = visual;
			m_visuals[index] = val;
		}
	}

	void CanvasController::OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		IVisual2D* hittest = HitTest(point);
		if (hittest != m_current)
		{
			m_current = hittest;
		}
		if (m_current != NULL && m_current->HitTest(point) >= 0)
		{
			m_current->Track(m_view.Handle(), point, FALSE);
		}
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

	void CanvasController::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&)
	{
		TinySize size(LOWORD(lParam), HIWORD(lParam));
		TinyAutoLock autolock(m_lock);
		m_display.Destory();
		if (!m_dx11.Resize(size.cx, size.cy))
		{
			LOG(ERROR) << "[OnSize] Resize FAIL";
		}
		if (!m_display.Create())
		{
			LOG(ERROR) << "[OnSize] Display Create Graphics FAIL";
		}
	}

	void CanvasController::OnSetCursor(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_view.Handle(), &point);
		if (m_current = HitTest(point))
		{
			if (m_current->SetCursor(m_view.Handle(), LOWORD(lParam)))
			{
				bHandled = TRUE;
			}
		}
	}

	void CanvasController::OnDraw()
	{
		for (;;)
		{
			{
				TinyAutoLock autolock(m_lock);
				m_display.BeginDraw();
				for (INT i = 0; i < m_visuals.GetSize(); i++)
				{
					IVisual2D* visual2D = m_visuals[i];
					visual2D->Tick();
					DX11Image2D* val = visual2D->GetVisual2D();
					m_display.DrawImage(*val);
					if (visual2D->IsKindOf(RUNTIME_CLASS(MonitorVisual2D)))
					{
						MonitorVisual2D* ps = static_cast<MonitorVisual2D*>(visual2D);
						m_display.DrawImage(ps->GetCursor());
					}
				}
				m_display.EndDraw();
				m_dx11.Present();
			}
			Sleep(30);
		}
	}

	IVisual2D* CanvasController::HitTest(const TinyPoint& pos)
	{
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual2D* visual2D = m_visuals[i];
			TinyRectangle rectangle;
			visual2D->GetTrackerRect(&rectangle);
			if (rectangle.PtInRect(pos))
			{
				return visual2D;
			}
		}
		return NULL;
	}
}
