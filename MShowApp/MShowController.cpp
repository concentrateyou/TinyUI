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
		string box;
		box.resize(MAX_PATH);
		GetModuleFileName(NULL, &box[0], MAX_PATH);
		box = box.substr(0, box.find_last_of("\\", string::npos, 1));
		string vs = box + "\\box.png";
		ASSERT(PathFileExists(vs.c_str()));
		HRESULT hRes = CreateD2DBitmapFromFile(StringToWString(vs).c_str(), m_d2d.GetContext(), &m_bitmapBox);
		if (hRes != S_OK)
			return FALSE;
		MFLVScene* pFLV1 = new MFLVScene(this);
		if (!pFLV1->Initialize(m_d2d, "rtmp://live.hkstv.hk.lxdns.com/live/hks"))
		{
			SAFE_DELETE(pFLV1);
			return FALSE;
		}
		if (!pFLV1->Submit())
		{
			SAFE_DELETE(pFLV1);
			return FALSE;
		}

		/*MFLVScene* pFLV2 = new MFLVScene(this);
		if (!pFLV2->Initialize(m_d2d, "rtmp://10.10.13.98/live/lb_junlvjuchang_720p"))
		{
			SAFE_DELETE(pFLV2);
			return FALSE;
		}
		if (!pFLV2->Submit())
		{
			SAFE_DELETE(pFLV2);
			return FALSE;
		}

		MFLVScene* pFLV3 = new MFLVScene(this);
		if (!pFLV3->Initialize(m_d2d, "rtmp://live.hkstv.hk.lxdns.com/live/hks"))
		{
			SAFE_DELETE(pFLV3);
			return FALSE;
		}
		if (!pFLV3->Submit())
		{
			SAFE_DELETE(pFLV3);
			return FALSE;
		}*/

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
		/*this->Add(pFLV2);
		this->Add(pFLV3);*/
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
		m_bitmapBox.Release();
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
	BOOL MShowController::Remove(MElement* element)
	{
		m_lock.Lock();
		BOOL bRes = m_scenes.Remove(element);
		m_lock.Unlock();
		return bRes;
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
		m_lock.Lock();
		if (m_d2d.BeginDraw())
		{
			for (UINT i = 0;i < m_scenes.GetSize();i++)
			{
				m_scenes[i]->Draw(m_d2d);
				if (m_scenes[i] == m_lastElement)
				{
					UINT mask = m_scenes[i]->GetHandleMask();
					for (INT j = 0; j < 8; ++j)
					{
						if (mask & (1 << j))
						{
							TinyRectangle rectangle;
							m_scenes[i]->GetHandleRect((TrackerHit)j, &rectangle);
							TinyPoint pos = rectangle.Position();
							TinySize size = rectangle.Size();
							D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
							D2D_RECT_F src = { 0.0F,0.0F,6.0,6.0F };
							if (m_bitmapBox != NULL)
							{
								m_d2d.GetContext()->DrawBitmap(m_bitmapBox, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
							}
						}
					}
				}
			}
			m_d2d.EndDraw();
		}
		m_lock.Unlock();
	}


}
