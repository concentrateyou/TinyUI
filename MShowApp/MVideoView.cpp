#include "stdafx.h"
#include "MVideoView.h"
#include "WICTexture.h"
#include "resource.h"

namespace MShow
{
#define IDM_ADD_VIDEO		105
#define IDM_REMOVE_VIDEO	106

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
	MVideoView::MVideoView(MPreviewController& controller, DWORD dwIndex)
		:m_player(m_dx2d, BindCallback(&MVideoView::OnVideo, this)),
		m_controller(controller),
		m_dwIndex(dwIndex)
	{
	}

	MVideoView::~MVideoView()
	{
	}
	LRESULT MVideoView::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	BOOL MVideoView::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD MVideoView::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD);
	}

	DWORD MVideoView::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR MVideoView::RetrieveClassName()
	{
		return TEXT("VideoView");
	}

	LPCSTR MVideoView::RetrieveTitle()
	{
		return TEXT("VideoView");
	}

	HICON MVideoView::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT MVideoView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		RECT s = { 0 };
		this->GetClientRect(&s);
		m_dx2d.Initialize(m_hWND, TO_CX(s), TO_CY(s));
		m_menu.CreatePopupMenu();
		m_menu.AppendMenu(MF_STRING, IDM_ADD_VIDEO, TEXT("Ìí¼Ó"));
		m_menu.AppendMenu(MF_STRING, IDM_REMOVE_VIDEO, TEXT("É¾³ý"));
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MVideoView::OnMenuClick));
		m_menu.EVENT_CLICK += m_onMenuClick;
		return FALSE;
	}

	LRESULT MVideoView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_menu.EVENT_CLICK -= m_onMenuClick;
		m_player.Close();
		return FALSE;
	}

	LRESULT MVideoView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);
		::FillRect(hDC, &s.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT MVideoView::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MVideoView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT MVideoView::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		this->ClientToScreen(&point);
		m_menu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_hWND, NULL);
		return FALSE;
	}

	LRESULT MVideoView::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_controller.Add(m_model))
		{
			m_controller.Draw();
		}
		return FALSE;
	}

	void MVideoView::DrawView()
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

	void MVideoView::OnVideo(BYTE* bits, LONG size)
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
				HANDLE handle = m_controller.GetSignal(m_dwIndex);
				SetEvent(handle);
			}
		}
	}

	void MVideoView::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case IDM_ADD_VIDEO:
			OnAdd();
			break;
		case IDM_REMOVE_VIDEO:
			OnRemove();
			break;
		}
	}

	void MVideoView::OnAdd()
	{
		MVideoDialog dlg;
		if (dlg.DoModal(m_hWND, IDD_VIDEO) == IDOK)
		{
			this->OnRemove();
			TinyString val = dlg.GetAddress();
			m_player.Close();
			if (m_player.Open(val.STR()))
			{
				m_model.Reset(new MFLVModel(m_controller, m_dwIndex));
				TinySize videoSize = m_player.GetVideoSize();
				m_model->Initialize(videoSize);
			}
		}
	}

	void MVideoView::OnRemove()
	{
		if (m_model != NULL)
		{
			if (m_controller.Remove(m_model))
			{
				HANDLE handle = m_controller.GetSignal(m_dwIndex);
				SetEvent(handle);
			}
			m_player.Close();
			m_model.Reset(NULL);
			this->Invalidate();
		}
	}

	DX2D& MVideoView::GetD2D()
	{
		return m_dx2d;
	}
}


