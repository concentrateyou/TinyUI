#include "stdafx.h"
#include "MShowController.h"

namespace MShow
{
	MShowController::MShowController(DXView& view)
		:m_view(view),
		m_lastElement(NULL)
	{
	}


	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize()
	{
		RECT s = { 0 };
		::GetWindowRect(m_view.Handle(), &s);
		TinySize size = { TO_CX(s) ,TO_CY(s) };
		if (!m_d2d.Initialize(m_view.Handle(), size.cx, size.cy))
			return FALSE;
		if (!m_queue.Create())
			return FALSE;
		MFLVScene* pFLV1 = new MFLVScene(this);
		if (!pFLV1->Initialize(m_d2d, "rtmp://10.121.86.127/live/test_360p"))
		{
			SAFE_DELETE(pFLV1);
			return FALSE;
		}
		if (!pFLV1->Submit())
		{
			SAFE_DELETE(pFLV1);
			return FALSE;
		}

		MFLVScene* pFLV2 = new MFLVScene(this);
		if (!pFLV2->Initialize(m_d2d, "rtmp://10.121.86.127/live/test_360p_1"))
		{
			SAFE_DELETE(pFLV2);
			return FALSE;
		}
		if (!pFLV2->Submit())
		{
			SAFE_DELETE(pFLV2);
			return FALSE;
		}

		MGIFScene* pGIF = new MGIFScene(this);
		if (!pGIF->Initialize(m_d2d, "D:\\timg.gif"))
		{
			SAFE_DELETE(pGIF);
			return FALSE;
		}
		if (!pGIF->Submit())
		{
			SAFE_DELETE(pGIF);
			return FALSE;
		}

		this->Add(pFLV1);
		this->Add(pFLV2);
		this->Add(pGIF);

		m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSize));
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSetCursor));

		m_view.EVENT_SIZE += m_onSize;
		m_view.EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_view.EVENT_LBUTTONUP += m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE += m_onMouseMove;
		m_view.EVENT_MOUSELEAVE += m_onMouseLeave;
		m_view.EVENT_SETCURSOR += m_onSetCursor;
		return TRUE;
	}

	MElement* MShowController::HitTest(const TinyPoint& pos)
	{
		for (INT i = m_scenes.GetSize() - 1;i >= 0;i--)
		{
			if (m_scenes[i]->PtInRect(pos))
			{
				return m_scenes[i];
			}
		}
		return NULL;
	}

	void MShowController::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
	}

	void MShowController::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		MElement* element = HitTest(point);
		if (element != m_lastElement)
		{
			m_lastElement = element;
		}
		if (m_lastElement)
		{
			if (m_lastElement->HitTest(point) >= 0)
			{
				m_lastElement->Track(m_view.Handle(), point, FALSE);
			}
		}
	}

	void MShowController::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

	}

	void MShowController::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_lastElement = NULL;
	}

	void MShowController::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_view.Handle(), &point);
		if (m_lastElement = HitTest(point))
		{
			if (m_lastElement->SetCursor(m_view.Handle(), LOWORD(lParam)))
			{
				bHandled = TRUE;
			}
		}
	}

	void MShowController::Uninitialize()
	{
		m_view.EVENT_SIZE -= m_onSize;
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		for (UINT i = 0;i < m_scenes.GetSize();i++)
		{
			m_scenes[i]->Close();	
		}
		m_queue.Destory();
		for (UINT i = 0;i < m_scenes.GetSize();i++)
		{
			SAFE_DELETE(m_scenes[i]);
		}
	}

	TinyTimerQueue* MShowController::GetTimerQueue()
	{
		return &m_queue;
	}

	BOOL MShowController::Add(MElement* element)
	{
		m_lock.Lock();
		BOOL bRes = m_scenes.Add(element);
		m_lock.Unlock();
		return bRes;
	}
	void MShowController::Remove(MElement* element)
	{
		m_lock.Lock();
		m_scenes.Remove(element);
		m_lock.Unlock();
	}

	void MShowController::BringToTop(MElement* element)
	{
		m_lock.Lock();
		if (m_scenes.Lookup(element) >= 0)
		{
			m_scenes.Remove(element);
			m_scenes.Insert(0, element);
		}
		m_lock.Unlock();
	}
	void MShowController::BringToBottom(MElement* element)
	{
		m_lock.Lock();
		if (m_scenes.Lookup(element) >= 0)
		{
			m_scenes.Remove(element);
			m_scenes.Add(element);
		}
		m_lock.Unlock();
	}
	void MShowController::MoveUp(MElement* element)
	{
		m_lock.Lock();
		INT index = m_scenes.Lookup(element);
		if (index > 0)
		{
			m_scenes.Remove(element);
			m_scenes.Insert(index - 1, element);
		}
		m_lock.Unlock();
	}
	void MShowController::MoveDown(MElement* element)
	{
		m_lock.Lock();
		INT index = m_scenes.Lookup(element);
		if (index >= 0 && index < (m_scenes.GetSize() - 1))
		{
			m_scenes.Remove(element);
			m_scenes.Insert(index + 1, element);
		}
		m_lock.Unlock();
	}

	void MShowController::Draw(MElement* ps)
	{
		if (m_d2d.BeginDraw())
		{
			for (UINT i = 0;i < m_scenes.GetSize();i++)
			{
				m_scenes[i]->Draw(m_d2d);
			}
			m_d2d.EndDraw();
		}
	}
}
