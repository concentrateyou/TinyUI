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
		m_controller(controller)
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

	LRESULT VideoView::OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		MVideoDialog dlg;
		if (dlg.DoModal(m_hWND, IDD_VIDEO) == IDOK)
		{
			TinyString val = dlg.GetAddress();
			if (m_player.IsPlaying())
			{
				m_player.Close();
			}
			if (m_player.Open(val.STR()))
			{
				m_controller.Remove(m_model);
				m_model.Reset(new MFLVModel(m_controller));
				TinySize videoSize = m_player.GetVideoSize();
				ID2D1Bitmap1** bitmap = m_model->GetBitmap();
				HRESULT hRes = m_controller.GetD2D().GetContext()->CreateBitmap(D2D1::SizeU(videoSize.cx, videoSize.cy),
					(const void *)NULL,
					0,
					&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					bitmap);
				if (SUCCEEDED(hRes))
				{
					RECT s = { 0 };
					::GetClientRect(m_controller.GetView().Handle(), &s);
					TinySize size(TinySize(TO_CX(s), TO_CY(s)));
					m_model->SetSize(size);
					videoSize.cx = videoSize.cx / 2;
					videoSize.cy = videoSize.cy / 2;
					m_model->SetScale(videoSize);
				}
			}
		}
		return FALSE;
	}

	LRESULT VideoView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_controller.Add(m_model);
		return FALSE;
	}

	void VideoView::OnVideo(BYTE* bits, LONG size)
	{
		if (m_model != NULL)
		{
			ID2D1Bitmap1* ps = *(m_model->GetBitmap());
			if (ps != NULL)
			{
				TinySize s = m_player.GetVideoSize();
				ASSERT(s.cx * s.cy * 4 == size);
				ps->CopyFromMemory(NULL, bits, s.cx * 4);
			}
			m_controller.Draw(m_model);
		}
	}

	DX2D& VideoView::GetD2D()
	{
		return m_dx2d;
	}
}


