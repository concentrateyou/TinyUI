#include "stdafx.h"
#include "MPreviewController.h"
#include "WICTexture.h"

namespace MShow
{
	MPreviewController::MPreviewController(MPreviewView& view)
		:m_view(view),
		m_lastElement(NULL)
	{
	}


	MPreviewController::~MPreviewController()
	{
	}

	BOOL MPreviewController::Initialize()
	{
		RECT s = { 0 };
		::GetWindowRect(m_view.Handle(), &s);
		TinySize size = { TO_CX(s) ,TO_CY(s) };
		if (!m_dx2d.Initialize(m_view.Handle(), size.cx, size.cy))
			return FALSE;
		string box;
		box.resize(MAX_PATH);
		GetModuleFileName(NULL, &box[0], MAX_PATH);
		box = box.substr(0, box.find_last_of("\\", string::npos, 1));
		string vs = box + "\\box.png";
		ASSERT(PathFileExists(vs.c_str()));
		HRESULT hRes = CreateD2DBitmapFromFile(StringToWString(vs).c_str(), m_dx2d.GetContext(), &m_bitmapBox);
		if (hRes != S_OK)
			return FALSE;
		m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnSize));
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnSetCursor));
		m_view.EVENT_SIZE += m_onSize;
		m_view.EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_view.EVENT_LBUTTONUP += m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE += m_onMouseMove;
		m_view.EVENT_MOUSELEAVE += m_onMouseLeave;
		m_view.EVENT_SETCURSOR += m_onSetCursor;
		return TRUE;
	}

	MElement* MPreviewController::HitTest(const TinyPoint& pos)
	{
		for (INT i = m_models.GetSize() - 1;i >= 0;i--)
		{
			if (m_models[i]->PtInRect(pos))
			{
				return m_models[i];
			}
		}
		return NULL;
	}

	void MPreviewController::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
	}

	void MPreviewController::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	void MPreviewController::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MPreviewController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

	}

	void MPreviewController::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MPreviewController::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_lastElement = NULL;
	}

	void MPreviewController::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	void MPreviewController::Uninitialize()
	{
		m_bitmapBox.Release();
		m_view.EVENT_SIZE -= m_onSize;
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		m_lock.Lock();
		m_models.RemoveAll();
		m_lock.Unlock();
	}

	BOOL MPreviewController::Add(MElement* element)
	{
		m_lock.Lock();
		BOOL bRes = FALSE;
		if (m_models.Lookup(element) < 0)
		{
			bRes = m_models.Add(element);
		}
		m_lock.Unlock();
		return bRes;
	}
	BOOL MPreviewController::Remove(MElement* element)
	{
		m_lock.Lock();
		BOOL bRes = m_models.Remove(element);
		m_lock.Unlock();
		return bRes;
	}

	void MPreviewController::BringToTop(MElement* element)
	{
		m_lock.Lock();
		if (m_models.Lookup(element) >= 0)
		{
			m_models.Remove(element);
			m_models.Insert(0, element);
		}
		m_lock.Unlock();
	}
	void MPreviewController::BringToBottom(MElement* element)
	{
		m_lock.Lock();
		if (m_models.Lookup(element) >= 0)
		{
			m_models.Remove(element);
			m_models.Add(element);
		}
		m_lock.Unlock();
	}
	void MPreviewController::MoveUp(MElement* element)
	{
		m_lock.Lock();
		INT index = m_models.Lookup(element);
		if (index > 0)
		{
			m_models.Remove(element);
			m_models.Insert(index - 1, element);
		}
		m_lock.Unlock();
	}
	void MPreviewController::MoveDown(MElement* element)
	{
		m_lock.Lock();
		INT index = m_models.Lookup(element);
		if (index >= 0 && index < (m_models.GetSize() - 1))
		{
			m_models.Remove(element);
			m_models.Insert(index + 1, element);
		}
		m_lock.Unlock();
	}
	DX2D& MPreviewController::GetD2D()
	{
		return m_dx2d;
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}

	void MPreviewController::Draw(MElement* ps)
	{
		m_lock.Lock();
		if (m_dx2d.BeginDraw())
		{
			for (INT i = 0;i < m_models.GetSize();i++)
			{
				m_models[i]->Draw();
				if (m_bitmapBox != NULL)
				{
					D2D_SIZE_F sizeF = m_bitmapBox->GetSize();
					if (m_models[i] == m_lastElement)
					{
						UINT mask = m_models[i]->GetHandleMask();
						for (INT j = 0; j < 8; ++j)
						{
							if (mask & (1 << j))
							{
								TinyRectangle rectangle;
								m_models[i]->GetHandleRect((TrackerHit)j, &rectangle);
								TinyPoint pos = rectangle.Position();
								TinySize size = rectangle.Size();
								D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
								D2D_RECT_F src = { 0.0F,0.0F,sizeF.width / 2,sizeF.height / 2 };
								m_dx2d.GetContext()->DrawBitmap(m_bitmapBox, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
							}
						}
					}
				}
			}
			m_dx2d.EndDraw();
		}
		m_lock.Unlock();
	}


}
