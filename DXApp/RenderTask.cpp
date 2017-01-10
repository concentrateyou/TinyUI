#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask()
	:m_currentElement(NULL)
{

}

RenderTask::~RenderTask()
{
	m_pWindow->EVENT_SIZE -= m_onSize;
	m_pWindow->EVENT_LBUTTONDOWN -= m_onLButtonDown;
	m_pWindow->EVENT_LBUTTONUP -= m_onLButtonUp;
	m_pWindow->EVENT_MOUSEMOVE -= m_onMouseMove;
	m_pWindow->EVENT_SETCURSOR -= m_onSetCursor;
}

BOOL RenderTask::Initialize(DXWindow* pWindow, INT cx, INT cy, DWORD dwFPS)
{
	ASSERT(pWindow);
	if (!m_graphics.Initialize(pWindow->Handle(), TinySize(cx, cy)))
		return FALSE;
	m_pWindow = pWindow;
	m_dwFPS = dwFPS;
	m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnSize));
	m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnLButtonDown));
	m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnLButtonUp));
	m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnMouseMove));
	m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnSetCursor));
	m_pWindow->EVENT_SIZE += m_onSize;
	m_pWindow->EVENT_LBUTTONDOWN += m_onLButtonDown;
	m_pWindow->EVENT_LBUTTONUP += m_onLButtonUp;
	m_pWindow->EVENT_MOUSEMOVE += m_onMouseMove;
	m_pWindow->EVENT_SETCURSOR += m_onSetCursor;
	return TRUE;
}
DX11Graphics2D*	 RenderTask::GetGraphics()
{
	return &m_graphics;
}

BOOL RenderTask::Close(DWORD dwMs)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMs);
}

BOOL RenderTask::Submit()
{
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	return TinyTaskBase::Submit(BindCallback(&RenderTask::OnMessagePump, this));
}

DWORD RenderTask::Render()
{
	m_timer.BeginTime();
	m_graphics.BeginScene();

	for (INT i = 0;i < m_scenes.GetSize();i++)
	{
		DX11Element* ps = m_scenes[i];
		if (ps->GetElementType() == IMAGE)
		{
			m_graphics.DrawImage(static_cast<DX11Image*>(ps));
		}
	}
	m_graphics.EndScene();
	m_timer.EndTime();
	return m_timer.GetMillisconds();
}

void RenderTask::OnRender(BYTE* bits, LONG size, FLOAT ts)
{
	EVENT_RENDER(bits, size, ts);
}

void RenderTask::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	INT cx = LOWORD(lParam);
	INT cy = HIWORD(lParam);
	TinyAutoLock lock(m_lock);
	m_graphics.Resize(TinySize(cx, cy));
}

void RenderTask::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	if (m_currentElement == NULL)
	{
		for (INT i = 0;i < m_scenes.GetSize();i++)
		{
			if (m_scenes[i]->m_rectangle.PtInRect(point))
			{
				m_currentElement = m_scenes[i];
				break;
			}
		}
	}
	if (m_currentElement)
	{
		ASSERT(m_pWindow);
		UINT hitHandle = m_currentElement->HitTest(point);
		if (hitHandle < 0)
		{
			m_currentElement->TrackRubberBand(m_pWindow->Handle(), point, TRUE);
			m_currentElement->m_rectangle.NormalizeRect();
		}
		else
		{
			m_currentElement->Track(m_pWindow->Handle(), point, TRUE);
		}
	}
}

void RenderTask::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

}

void RenderTask::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

}

void RenderTask::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	ASSERT(m_pWindow);
	TinyPoint point;
	GetCursorPos(&point);
	::ScreenToClient(m_pWindow->Handle(), &point);
	if (m_currentElement == NULL)
	{
		for (INT i = 0;i < m_scenes.GetSize();i++)
		{
			if (m_scenes[i]->m_rectangle.PtInRect(point))
			{
				m_currentElement = m_scenes[i];
				break;
			}
		}
	}
	if (m_currentElement &&
		m_currentElement->SetCursor(m_pWindow->Handle(), LOWORD(lParam)))
	{
		bHandled = TRUE;
	}
}

BOOL RenderTask::Add(DX11Element* element)
{
	TinyAutoLock lock(m_lock);
	return m_scenes.Add(element);
}
void RenderTask::Remove(DX11Element* element)
{
	TinyAutoLock lock(m_lock);
	m_scenes.Remove(element);
}

BOOL RenderTask::Contain(DX11Element* element)
{
	TinyAutoLock lock(m_lock);
	return m_scenes.Lookup(element) >= 0;
}

void RenderTask::OnMessagePump()
{
	DWORD dwTime = 0;
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		s = dwTime > s ? 0 : s - dwTime;
		if (m_close.Lock(s))
		{
			for (INT i = 0;i < m_scenes.GetSize();i++)
			{
				m_scenes[i]->EndScene();
			}
			break;
		}
		TinyAutoLock lock(m_lock);
		dwTime = this->Render();
	}
}
