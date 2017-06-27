#include "stdafx.h"
#include "MVideoController.h"
#include "MPreviewController.h"
#include "MVideoElement.h"
#include "MShowApp.h"
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
	MVideoController::MVideoController(MVideoView& view)
		:m_view(view),
		m_video(NULL),
		m_player(BindCallback(&MVideoController::OnVideo, this))
	{
		m_onLButtonDBClick.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MVideoController::OnLButtonDBClick));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MVideoController::OnRButtonDown));
		m_view.EVENT_LDBCLICK += m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MVideoController::OnMenuClick));
		m_popup.EVENT_CLICK += m_onMenuClick;
		m_onVolume.Reset(new Delegate<void(DWORD)>(this, &MVideoController::OnVolume));
	}

	MVideoController::~MVideoController()
	{
		m_view.EVENT_LDBCLICK -= m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_popup.EVENT_CLICK -= m_onMenuClick;
		m_popup.DestroyMenu();
	}

	BOOL MVideoController::Initialize()
	{
		TinyRectangle rectangle;
		m_view.GetClientRect(&rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
			return FALSE;
		m_popup.CreatePopupMenu();
		m_popup.AppendMenu(MF_STRING, 1, TEXT("添加"));
		m_popup.AppendMenu(MF_STRING, 2, TEXT("删除"));
		m_signal.CreateEvent();
		return TRUE;
	}

	BOOL MVideoController::Open(LPCSTR pzURL)
	{
		TinySize size;
		TinyRectangle rectangle;
		if (!m_player.Open(m_view.Handle(), pzURL))
			goto L_ERROR;
		size = m_player.GetSize();
		if (!m_copy2D.Create(m_graphics.GetDX11(), size, TRUE))
			goto L_ERROR;
		m_copy2D.SetScale(size);
		if (!m_video2D.Create(m_graphics.GetDX11(), size, FALSE, FALSE))
			goto L_ERROR;
		m_view.GetClientRect(&rectangle);
		m_video2D.SetScale(rectangle.Size());
		return TRUE;
	L_ERROR:
		m_player.Close();
		m_copy2D.Destory();
		m_video2D.Destory();
		return FALSE;
	}

	BOOL MVideoController::Close()
	{
		if (m_player.Close())
		{
			m_copy2D.Destory();
			m_video2D.Destory();
			return TRUE;
		}
		return FALSE;
	}

	TinyString	MVideoController::GetURL() const
	{
		return m_player.GetURL();
	}

	HANDLE MVideoController::GetHandle()
	{
		DX11Texture2D* ps = m_copy2D.GetTexture2D();
		if (ps && !ps->IsEmpty())
		{
			return ps->GetHandle();
		}
		return NULL;
	}

	void MVideoController::OnVideo(BYTE* bits, LONG size)
	{
		TinySize videoSize = m_player.GetSize();
		if (m_video2D.Copy(m_graphics.GetDX11(), bits, size, videoSize.cx * 4))
		{
			m_graphics.GetDX11().SetRenderTexture2D(NULL);
			m_graphics.GetDX11().GetRender2D()->BeginDraw();
			m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
			m_graphics.GetDX11().GetRender2D()->EndDraw();
			m_graphics.Present();
			m_copy2D.Copy(m_graphics.GetDX11(), &m_video2D);
			m_signal.SetEvent();
		}
	}

	void MVideoController::OnAdd()
	{
		MVideoDialog dlg;
		if (dlg.DoModal(m_view.Handle(), IDD_VIDEO) == IDOK)
		{
			this->OnRemove();
			TinyString szName = dlg.GetAddress();
			if (!szName.IsEmpty())
			{
				this->Open(szName.STR());
			}
		}
	}

	void MVideoController::OnRemove()
	{
		MPreviewController* preview = MShowApp::Instance().GetController().GetPreviewController();
		if (preview != NULL && m_video != NULL)
		{
			m_video->Deallocate(preview->Graphics().GetDX11());
			SAFE_DELETE(m_video);
		}
		this->Close();
	}

	void MVideoController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_view.ClientToScreen(&point);
		m_popup.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
	}

	void MVideoController::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		MPreviewController* preview = MShowApp::Instance().GetController().GetPreviewController();
		if (preview != NULL)
		{
			if (m_video == NULL)
			{
				m_video = new MVideoElement(*this);
				if (m_video->Allocate(preview->Graphics().GetDX11()))
				{
					preview->Add(m_video);
				}
			}
			else
			{
				preview->Add(m_video);
			}
		}
	}

	void MVideoController::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case 1:
			OnAdd();
			break;
		case 2:
			OnRemove();
			break;
		}
	}

	void MVideoController::OnVolume(DWORD pos)
	{
		if (pos == 0)
		{
			m_player.SetVolume(-10000);
		}
		else
		{
			LONG volume = static_cast<LONG>(floorf(2000.0F * log10f((FLOAT)(pos) / (FLOAT)100) + 0.5F));
			m_player.SetVolume(volume);
		}

	}
}

