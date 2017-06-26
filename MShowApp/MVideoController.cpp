#include "stdafx.h"
#include "MVideoController.h"
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
		m_player(BindCallback(&MVideoController::OnVideo, this))
	{

	}

	MVideoController::~MVideoController()
	{

	}

	BOOL MVideoController::Initialize()
	{
		TinyRectangle s;
		m_view.GetClientRect(&s);
		if (!m_graphics.Initialize(m_view.Handle(), s.Size()))
			return FALSE;
		return TRUE;
	}

	BOOL MVideoController::Open(LPCSTR pzURL)
	{
		TinySize size;
		TinyRectangle rectangle;
		if (!m_player.Open(m_view.Handle(), pzURL))
			goto _ERROR;
		size = m_player.GetSize();
		if (!m_copy2D.Create(m_graphics.GetDX11(), size, TRUE))
			goto _ERROR;
		m_copy2D.SetScale(size);
		if (!m_video2D.Create(m_graphics.GetDX11(), size, FALSE, FALSE))
			goto _ERROR;
		m_view.GetClientRect(&rectangle);
		m_copy2D.SetScale(rectangle.Size());
		return TRUE;
	_ERROR:
		m_player.Close();
		return FALSE;
	}

	BOOL MVideoController::Close()
	{
		return m_player.Close();
	}

	HANDLE MVideoController::GetHandle()
	{
		DX11Texture2D* ps = m_video2D.GetTexture2D();
		if (ps && !ps->IsEmpty())
		{
			return ps->GetHandle();
		}
		return NULL;
	}

	void MVideoController::OnVideo(BYTE* bits, LONG size)
	{
		TinyPerformanceTimer timer;
		timer.BeginTime();
		TinySize videoSize = m_player.GetSize();
		if (m_video2D.Copy(m_graphics.GetDX11(), bits, size, videoSize.cx * 4))
		{
			m_graphics.GetDX11().SetRenderTexture2D(NULL);
			m_graphics.GetDX11().GetRender2D()->BeginDraw();
			m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
			m_graphics.GetDX11().GetRender2D()->EndDraw();
			m_graphics.Present();
			m_copy2D.Copy(m_graphics.GetDX11(), &m_video2D);
		}
		timer.EndTime();
		TRACE("Cost:%lld\n", timer.GetMillisconds());
	}
}

