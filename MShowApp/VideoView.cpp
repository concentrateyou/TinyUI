#include "stdafx.h"
#include "VideoView.h"
#include "WICTexture.h"
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
		if (m_dx2d.Initialize(m_hWND, TO_CX(s), TO_CY(s)))
		{
			/*string val;
			val.resize(MAX_PATH);
			GetModuleFileName(NULL, &val[0], MAX_PATH);
			val = val.substr(0, val.find_last_of("\\", string::npos, 1));
			string vs = val + "\\close.png";
			ASSERT(PathFileExists(vs.c_str()));
			HRESULT hRes = CreateD2DBitmapFromFile(StringToWString(vs).c_str(), m_dx2d.GetContext(), &m_bitmapClose);
			ASSERT(SUCCEEDED(hRes));*/
		}
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
				if (m_model->Initialize(videoSize))
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

	void VideoView::DrawView()
	{
		ID2D1Bitmap1* bitmap = m_player.GetBitmap();
		if (bitmap != NULL)
		{
			m_dx2d.BeginDraw();
			TinyRectangle s;
			::GetClientRect(m_hWND, &s);
			TinySize videoSize = m_player.GetVideoSize();
			TinyPoint pos = s.Position();
			TinySize size = s.Size();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<FLOAT>(videoSize.cx), static_cast<FLOAT>(videoSize.cy) };
			m_dx2d.GetContext()->DrawBitmap(bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			m_dx2d.EndDraw();
		}
	}

	void VideoView::OnVideo(BYTE* bits, LONG size)
	{
		if (m_model != NULL)
		{
			this->DrawView();
			ID2D1Bitmap1* bitmap = m_model->GetBitmap();
			if (bitmap != NULL)
			{
				TinySize s = m_player.GetVideoSize();
				ASSERT(s.cx * s.cy * 4 == size);
				bitmap->CopyFromMemory(NULL, bits, s.cx * 4);
			}
			m_controller.Draw(m_model);
		}
	}

	DX2D& VideoView::GetD2D()
	{
		return m_dx2d;
	}
}


