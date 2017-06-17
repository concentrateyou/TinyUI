#include "stdafx.h"
#include "VideoView.h"
#include "resource.h"

namespace MShow
{
	MVideoDialog::MVideoDialog()
	{

	}
	MVideoDialog::~MVideoDialog()
	{

	}
	LRESULT MVideoDialog::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			m_address = m_textbox.GetText();
			if (EndDialog(LOWORD(wParam)))
			{

			}
			break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
	}
	LRESULT MVideoDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_textbox.SubclassDlgItem(IDC_EDIT_VIDEO, m_hWND);
		return FALSE;
	}
	TinyString MVideoDialog::GetAddress()
	{
		return m_address;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoView::VideoView(MPreviewController& controller)
		:m_player(m_dx2d, BindCallback(&VideoView::OnVideo, this)),
		m_controller(controller),
		m_model(controller)
	{
	}

	VideoView::~VideoView()
	{
	}
	LRESULT VideoView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL VideoView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD VideoView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD VideoView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR VideoView::RetrieveClassName()
	{
		return TEXT("VideoView");
	}

	LPCSTR VideoView::RetrieveTitle()
	{
		return TEXT("VideoView");
	}

	HICON VideoView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT VideoView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s = { 0 };
		this->GetClientRect(&s);
		m_dx2d.Initialize(m_hWND, TO_CX(s), TO_CY(s));
		return FALSE;
	}

	LRESULT VideoView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_player.Close();
		m_model.Release();
		return FALSE;
	}

	LRESULT VideoView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT VideoView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT VideoView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}


	LRESULT VideoView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		MVideoDialog dlg;
		if (dlg.DoModal(m_hWND, IDD_VIDEO) == IDOK)
		{
			TinyString val = dlg.GetAddress();
			if (m_player.Open(val.STR()))
			{
				TinySize size = m_player.GetSize();
				m_model.SetSize(size);
				size.cx = size.cx / 2;
				size.cy = size.cy / 2;
				m_model.SetScale(size);
				ID2D1Bitmap1* bitmap = m_model.GetBitmap();
				HRESULT hRes = m_dx2d.GetContext()->CreateBitmap(D2D1::SizeU(size.cx, size.cy),
					(const void *)NULL,
					0,
					&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&bitmap);
			}
		}
		return FALSE;
	}

	void VideoView::OnVideo(ID2D1Bitmap1* bitmap)
	{
		ID2D1Bitmap1* ps = m_model.GetBitmap();
		if (ps != NULL)
		{
			ps->CopyFromBitmap(NULL, bitmap, NULL);
		}
		m_controller.Draw(&m_model);
	}

	DX2D& VideoView::GetD2D()
	{
		return m_dx2d;
	}
}


