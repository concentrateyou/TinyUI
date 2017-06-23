#include "stdafx.h"
#include "MImageView.h"
#include "Control/TinyCommonDialog.h"

namespace MShow
{
#define IDM_ADD_IMAGE		107
#define IDM_REMOVE_IMAGE	108

	MImageView::MImageView(MPreviewController& controller, DWORD dwIndex)
		:m_controller(controller),
		m_dwIndex(dwIndex)
	{
	}

	MImageView::~MImageView()
	{
	}

	BOOL MImageView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MImageView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MImageView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MImageView::RetrieveClassName()
	{
		return TEXT("ImageView");
	}

	LPCSTR MImageView::RetrieveTitle()
	{
		return TEXT("ImageView");
	}

	HICON MImageView::RetrieveIcon()
	{
		return NULL;
	}

	DX2D& MImageView::GetD2D()
	{
		return m_dx2d;
	}

	LRESULT MImageView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s = { 0 };
		this->GetClientRect(&s);
		m_dx2d.Initialize(m_hWND, TO_CX(s), TO_CY(s));
		m_menu.CreatePopupMenu();
		m_menu.AppendMenu(MF_STRING, IDM_ADD_IMAGE, TEXT("Ìí¼Ó"));
		m_menu.AppendMenu(MF_STRING, IDM_REMOVE_IMAGE, TEXT("É¾³ý"));
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MImageView::OnMenuClick));
		m_menu.EVENT_CLICK += m_onMenuClick;
		return FALSE;
	}

	LRESULT MImageView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_bitmap1.Release();
		return FALSE;
	}

	LRESULT MImageView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MImageView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MImageView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MImageView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MImageView::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		this->ClientToScreen(&point);
		m_menu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_hWND, NULL);
		return FALSE;
	}

	LRESULT MImageView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_controller.Add(m_model))
		{
			m_controller.Draw();
		}
		return FALSE;
	}

	void MImageView::DrawView()
	{
		if (m_bitmap1 != NULL)
		{
			m_dx2d.BeginDraw();
			TinyRectangle s;
			::GetClientRect(m_hWND, &s);
			TinyPoint pos = s.Position();
			TinySize size = s.Size();
			D2D1_SIZE_F sizeF = m_bitmap1->GetSize();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,sizeF.width,sizeF.height };
			m_dx2d.GetContext()->DrawBitmap(m_bitmap1, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			m_dx2d.EndDraw();
		}
	}

	void MImageView::OnVideo(BYTE* bits, LONG size)
	{
		if (m_model != NULL && m_bitmap1 != NULL)
		{
			TinySize s = m_model->GetSize();
			m_bitmap1->CopyFromMemory(NULL, bits, s.cx * 4);
			this->DrawView();
			m_model->GetBitmap()->CopyFromMemory(NULL, bits, s.cx * 4);
			HANDLE handle = m_controller.GetSignal(m_dwIndex);
			SetEvent(handle);
		}
	}

	void MImageView::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case IDM_ADD_IMAGE:
			OnAdd();
			break;
		case IDM_REMOVE_IMAGE:
			OnRemove();
			break;
		}
	}

	void MImageView::OnAdd()
	{
		LPCTSTR lpszFilter = _T("Image Files(*.bmp, *.jpg, *.png, *.gif)|*.bmp;*.jpg;*.png;*.gif|All Files (*.*)|*.*||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			this->OnRemove();
			m_model.Reset(new MImageModel(m_controller, m_dwIndex, BindCallback(&MImageView::OnVideo, this)));
			if (m_model->Initialize(dlg.GetPathName().STR()))
			{
				TinySize imageSize = m_model->GetSize();
				HRESULT hRes = m_dx2d.GetContext()->CreateBitmap(D2D1::SizeU(imageSize.cx, imageSize.cy),
					(const void *)NULL,
					0,
					&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_bitmap1);
				if (SUCCEEDED(hRes))
				{
					m_model->Animate();
				}
			}
		}
	}

	void MImageView::OnRemove()
	{
		if (m_model != NULL)
		{
			if (m_controller.Remove(m_model))
			{
				HANDLE handle = m_controller.GetSignal(m_dwIndex);
				SetEvent(handle);
			}
			m_bitmap1.Release();
			m_model.Reset(NULL);
			this->Invalidate();
		}
	}
}


