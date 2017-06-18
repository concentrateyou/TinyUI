#include "stdafx.h"
#include "ImageView.h"
#include "Control/TinyCommonDialog.h"

namespace MShow
{
	ImageView::ImageView(MPreviewController& controller)
		:m_controller(controller)
	{
	}

	ImageView::~ImageView()
	{
	}

	BOOL ImageView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD ImageView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD ImageView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR ImageView::RetrieveClassName()
	{
		return TEXT("ImageView");
	}

	LPCSTR ImageView::RetrieveTitle()
	{
		return TEXT("ImageView");
	}

	HICON ImageView::RetrieveIcon()
	{
		return NULL;
	}

	DX2D& ImageView::GetD2D()
	{
		return m_dx2d;
	}

	LRESULT ImageView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s = { 0 };
		this->GetClientRect(&s);
		m_dx2d.Initialize(m_hWND, TO_CX(s), TO_CY(s));
		return FALSE;
	}

	LRESULT ImageView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_bitmap1.Release();
		return FALSE;
	}

	LRESULT ImageView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT ImageView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT ImageView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT ImageView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT ImageView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_controller.Add(m_model);
		return FALSE;
	}

	LRESULT ImageView::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPCTSTR lpszFilter = _T("Image Files(*.bmp, *.jpg, *.png, *.gif)|*.bmp;*.jpg;*.png;*.gif|All Files (*.*)|*.*||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_hWND) == IDOK)
		{
			m_controller.Remove(m_model);
			m_model.Reset(new MImageModel(m_controller, BindCallback(&ImageView::OnVideo, this)));
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
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	void ImageView::DrawView()
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

	void ImageView::OnVideo(BYTE* bits, LONG size)
	{
		if (m_model != NULL && m_bitmap1)
		{
			TinySize s = m_model->GetSize();
			m_bitmap1->CopyFromMemory(NULL, bits, s.cx * 4);
			m_model->GetBitmap()->CopyFromMemory(NULL, bits, s.cx * 4);
			this->DrawView();
			m_controller.Draw(m_model);
		}
	}
}


