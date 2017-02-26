#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

namespace DXApp
{
	RenderTask::RenderTask()
		:m_lastElement(NULL),
		m_bMouseTracking(NULL)
	{

	}

	RenderTask::~RenderTask()
	{
		m_menu.EVENT_CLICK -= m_onMenuClick;

		m_pWindow->EVENT_SIZE -= m_onSize;
		m_pWindow->EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_pWindow->EVENT_LBUTTONUP -= m_onLButtonUp;
		m_pWindow->EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_pWindow->EVENT_MOUSEMOVE -= m_onMouseMove;
		m_pWindow->EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_pWindow->EVENT_SETCURSOR -= m_onSetCursor;
	}

	BOOL RenderTask::Initialize(DXWindow* pWindow, INT cx, INT cy, DWORD dwFPS)
	{
		ASSERT(pWindow);
		if (!m_graphics.Initialize(pWindow->Handle(), TinySize(cx, cy)))
			return FALSE;

		for (INT i = 0;i < 8;i++)
		{
			m_handles[i].Create(m_graphics.GetDX11());
		}

		m_pWindow = pWindow;
		m_dwFPS = dwFPS;
		m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnSize));
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &RenderTask::OnSetCursor));
		m_pWindow->EVENT_SIZE += m_onSize;
		m_pWindow->EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_pWindow->EVENT_LBUTTONUP += m_onLButtonUp;
		m_pWindow->EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_pWindow->EVENT_MOUSEMOVE += m_onMouseMove;
		m_pWindow->EVENT_MOUSELEAVE += m_onMouseLeave;
		m_pWindow->EVENT_SETCURSOR += m_onSetCursor;

		m_menu.CreatePopupMenu();
		m_menu.AppendMenu(MF_STRING, IDM_MOVEUP, TEXT("上移"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVEDOWN, TEXT("下移"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVETOP, TEXT("移至顶部"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVEBOTTPM, TEXT("移至底部"));
		m_menu.AppendMenu(MF_STRING, IDM_REMOVE, TEXT("移除"));
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &RenderTask::OnMenuClick));
		m_menu.EVENT_CLICK += m_onMenuClick;

		return TRUE;
	}
	DX11Graphics2D&	 RenderTask::GetGraphics()
	{
		return m_graphics;
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
		m_graphics.BeginDraw();
		for (INT i = 0;i < m_scenes.GetSize();i++)
		{
			DX11Element2D* ps = m_scenes[i];
			if (ps->IsKindof(RUNTIME_CLASS(DX11Image2D)))
			{
				DX11Image2D* pImage = static_cast<DX11Image2D*>(ps);
				if (ps == m_lastElement)
				{
					UINT mask = pImage->GetHandleMask();
					for (INT i = 0; i < 8; ++i)
					{
						if (mask & (1 << i))
						{
							TinyRectangle rectangle;
							pImage->GetHandleRect((TrackerHit)i, &rectangle);
							m_graphics.DrawRectangle(&m_handles[i], rectangle);
						}
					}
				}
				m_graphics.DrawImage(pImage);
			}
		}
		m_graphics.EndDraw();
		m_timer.EndTime();
		return m_timer.GetMillisconds();
	}

	void RenderTask::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
		if (cx > 0 && cy > 0)
		{
			m_graphics.Lock();
			m_graphics.Resize(TinySize(cx, cy));
			m_graphics.Unlock();
		}
	}

	void RenderTask::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_pWindow);
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		DX11Element2D* element = HitTest(point);
		if (element != m_lastElement)
		{
			m_lastElement = element;
		}
		if (m_lastElement)
		{
			if (m_lastElement->HitTest(point) >= 0)
			{
				m_lastElement->Track(m_pWindow->Handle(), point, FALSE);
			}
		}
	}

	void RenderTask::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void RenderTask::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_pWindow);
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		DX11Element2D* element = HitTest(point);
		if (element != m_lastElement)
		{
			m_lastElement = element;
		}
		if (m_lastElement)
		{
			m_pWindow->ClientToScreen(&point);
			m_menu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_pWindow->Handle(), NULL);
		}
	}

	void RenderTask::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		/*if (!m_bMouseTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_pWindow->Handle();
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 10;
			m_bMouseTracking = _TrackMouseEvent(&tme);
		}*/
	}

	void RenderTask::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_bMouseTracking)
			m_bMouseTracking = FALSE;
		m_lastElement = NULL;
	}

	void RenderTask::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ASSERT(m_pWindow);
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_pWindow->Handle(), &point);
		if (m_lastElement = HitTest(point))
		{
			if (m_lastElement->SetCursor(m_pWindow->Handle(), LOWORD(lParam)))
			{
				bHandled = TRUE;
			}
		}
	}

	void RenderTask::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case IDM_MOVEDOWN:
			MoveDown(m_lastElement);
			break;
		case IDM_MOVEUP:
			MoveUp(m_lastElement);
			break;
		case IDM_MOVETOP:
			BringToTop(m_lastElement);
			break;
		case IDM_MOVEBOTTPM:
			BringToBottom(m_lastElement);
			break;
		case IDM_REMOVE:
		{
			if (m_lastElement)
			{
				Remove(m_lastElement);
				m_lastElement->Clear(m_graphics.GetDX11());
				m_lastElement = NULL;
			}
		}
		break;
		}
	}

	BOOL RenderTask::Add(DX11Element2D* element)
	{
		m_graphics.Lock();
		BOOL bRes = m_scenes.Add(element);
		m_graphics.Unlock();
		return bRes;
	}
	void RenderTask::Remove(DX11Element2D* element)
	{
		m_graphics.Lock();
		m_scenes.Remove(element);
		m_graphics.Unlock();
	}

	void RenderTask::BringToTop(DX11Element2D* element)
	{
		m_graphics.Lock();
		if (m_scenes.Lookup(element) >= 0)
		{
			m_scenes.Remove(element);
			m_scenes.Insert(0, element);
		}
		m_graphics.Unlock();
	}
	void RenderTask::BringToBottom(DX11Element2D* element)
	{
		m_graphics.Lock();
		if (m_scenes.Lookup(element) >= 0)
		{
			m_scenes.Remove(element);
			m_scenes.Add(element);
		}
		m_graphics.Unlock();
	}
	void RenderTask::MoveUp(DX11Element2D* element)
	{
		m_graphics.Lock();
		INT index = m_scenes.Lookup(element);
		if (index > 0)
		{
			m_scenes.Remove(element);
			m_scenes.Insert(index - 1, element);
		}
		m_graphics.Unlock();
	}
	void RenderTask::MoveDown(DX11Element2D* element)
	{
		m_graphics.Lock();
		INT index = m_scenes.Lookup(element);
		if (index >= 0 && index < m_scenes.GetSize() - 1)
		{
			m_scenes.Remove(element);
			m_scenes.Insert(index + 1, element);
		}
		m_graphics.Unlock();
	}
	DX11Element2D* RenderTask::HitTest(const TinyPoint& pos)
	{
		for (INT i = 0;i < m_scenes.GetSize();i++)
		{
			if (m_scenes[i]->m_rectangle.PtInRect(pos))
			{
				return m_scenes[i];
			}
		}
		return NULL;
	}

	void RenderTask::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
	{
	}

	BOOL RenderTask::Contain(DX11Element2D* element)
	{
		m_graphics.Lock();
		BOOL bRes = m_scenes.Lookup(element) >= 0;
		m_graphics.Unlock();
		return bRes;
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
				for (INT i = 0; i < 8; ++i)
				{
					m_handles[i].Destory();
				}
				for (INT i = 0;i < m_scenes.GetSize();i++)
				{
					m_scenes[i]->Clear(m_graphics.GetDX11());
				}
				break;
			}
			m_graphics.Lock();
			dwTime = this->Render();
			m_graphics.Unlock();
		}
	}
}
