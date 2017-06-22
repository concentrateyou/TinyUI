#include "stdafx.h"
#include "MPreviewController.h"
#include "WICTexture.h"

namespace MShow
{
#define IDM_MOVEUP		100
#define IDM_MOVEDOWN	101
#define IDM_MOVETOP		102
#define IDM_MOVEBOTTPM	103
#define IDM_REMOVE		104

	MPreviewController::MPreviewController(MPreviewView& view)
		:m_view(view),
		m_lastElement(NULL)
	{
	}


	MPreviewController::~MPreviewController()
	{
	}

	BOOL MPreviewController::Initialize(const TinySize& pulgSize)
	{
		m_pulgSize = pulgSize;
		TinyRectangle rectangle;
		::GetClientRect(m_view.Handle(), &rectangle);
		m_size = rectangle.Size();
		if (!m_dx2d.Initialize(m_view.Handle(), m_size.cx, m_size.cy))
			return FALSE;
		string szName;
		szName.resize(MAX_PATH);
		GetModuleFileName(NULL, &szName[0], MAX_PATH);
		szName = szName.substr(0, szName.find_last_of("\\", string::npos, 1));
		string vs = szName + "\\box.png";
		ASSERT(PathFileExists(vs.c_str()));
		HRESULT hRes = CreateD2DBitmapFromFile(StringToWString(vs).c_str(), m_dx2d.GetContext(), &m_box);
		if (hRes != S_OK)
			return FALSE;
		D2D1_BITMAP_PROPERTIES1 props1 = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
		D2D1_SIZE_U sizeU = { pulgSize.cx, pulgSize.cy };
		hRes = m_dx2d.GetContext()->CreateBitmap(
			sizeU,
			nullptr,
			0,
			&props1,
			&m_bitmap
		);
		if (hRes != S_OK)
			return FALSE;
		props1 = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
		hRes = m_dx2d.GetContext()->CreateBitmap(
			sizeU,
			nullptr,
			0,
			&props1,
			&m_bitmapCopy
		);
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

		m_menu.CreatePopupMenu();
		m_menu.AppendMenu(MF_STRING, IDM_MOVEUP, TEXT("上移"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVEDOWN, TEXT("下移"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVETOP, TEXT("移至顶部"));
		m_menu.AppendMenu(MF_STRING, IDM_MOVEBOTTPM, TEXT("移至底部"));
		m_menu.AppendMenu(MF_STRING, IDM_REMOVE, TEXT("移除"));
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MPreviewController::OnMenuClick));
		m_menu.EVENT_CLICK += m_onMenuClick;

		return TRUE;
	}

	MElement* MPreviewController::HitTest(const TinyPoint& pos)
	{
		for (INT i = 0;i < m_models.GetSize();i++)
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
		m_size.cx = cx;
		m_size.cy = cy;
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
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		MElement* element = HitTest(point);
		if (element != m_lastElement)
		{
			m_lastElement = element;
		}
		if (m_lastElement)
		{
			m_view.ClientToScreen(&point);
			m_menu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
		}
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

	void MPreviewController::OnMenuClick(void*, INT wID)
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
				m_lastElement = NULL;
			}
		}
		break;
		}
	}

	void MPreviewController::Uninitialize()
	{
		TinyAutoLock lock(m_lock);
		m_box.Release();
		m_bitmap.Release();
		m_bitmapCopy.Release();
		m_view.EVENT_SIZE -= m_onSize;
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		m_models.RemoveAll();
	}

	BOOL MPreviewController::Add(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		BOOL bRes = FALSE;
		if (element != NULL)
		{
			if (m_models.Lookup(element) < 0)
			{
				bRes = m_models.Add(element);
			}
		}
		return bRes;
	}
	BOOL MPreviewController::Remove(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		return m_models.Remove(element);
	}

	void MPreviewController::BringToTop(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		if (m_models.Lookup(element) >= 0)
		{
			m_models.Remove(element);
			m_models.Insert(0, element);
		}
	}
	void MPreviewController::BringToBottom(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		if (m_models.Lookup(element) >= 0)
		{
			m_models.Remove(element);
			m_models.Add(element);
		}
	}
	void MPreviewController::MoveUp(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		INT index = m_models.Lookup(element);
		if (index > 0)
		{
			m_models.Remove(element);
			m_models.Insert(index - 1, element);
		}
	}
	void MPreviewController::MoveDown(MElement* element)
	{
		TinyAutoLock lock(m_lock);
		INT index = m_models.Lookup(element);
		if (index >= 0 && index < (m_models.GetSize() - 1))
		{
			m_models.Remove(element);
			m_models.Insert(index + 1, element);
		}
	}
	DX2D& MPreviewController::GetD2D()
	{
		return m_dx2d;
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}

	ID2D1Bitmap1* MPreviewController::GetBitmap()
	{
		return m_bitmapCopy;
	}

	void MPreviewController::Draw(MElement* ps)
	{
		m_dx2d.Enter();
		if (m_dx2d.BeginDraw(m_bitmap))
		{
			for (INT i = m_models.GetSize() - 1;i >= 0;i--)
			{
				m_models[i]->Draw(FLOAT(m_pulgSize.cx) / FLOAT(m_size.cx), FLOAT(m_pulgSize.cy) / FLOAT(m_size.cy));
			}
			m_dx2d.EndDraw();
			m_bitmapCopy->CopyFromBitmap(NULL, m_bitmap, NULL);
		}
		if (m_dx2d.BeginDraw())
		{
			for (INT i = m_models.GetSize() - 1;i >= 0;i--)
			{
				m_models[i]->Draw(1.0F, 1.0F);
				if (m_box != NULL)
				{
					D2D_SIZE_F sizeF = m_box->GetSize();
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
								m_dx2d.GetContext()->DrawBitmap(m_box, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
							}
						}
					}
				}
			}
			m_dx2d.EndDraw();
		}
		m_dx2d.Leave();
	}
}
