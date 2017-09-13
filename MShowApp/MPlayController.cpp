#include "stdafx.h"
#include "MPlayController.h"

namespace MShow
{
	MPlayController::MPlayController(MPlayView& view)
		:m_view(view),
		m_player(BindCallback(&MPlayController::OnVideo, this))
	{
		m_onVolume.Reset(new Delegate<void(DWORD)>(this, &MPlayController::OnVolume));
	}

	MPlayController::~MPlayController()
	{

	}

	BOOL MPlayController::Initialize()
	{
		TinyRectangle rectangle;
		m_view.GetClientRect(&rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
			return FALSE;
		return TRUE;
	}

	BOOL MPlayController::Open(LPCSTR pzURL)
	{
		TinySize size;
		TinyRectangle rectangle;
		if (!m_player.Open(m_view.Handle(), pzURL))
			goto _ERROR;
		size = m_player.GetSize();
		if (!m_video2D.Create(m_graphics.GetDX11(), size, TRUE))
			goto _ERROR;
		m_view.GetClientRect(&rectangle);
		m_video2D.SetScale(rectangle.Size());
		return TRUE;
	_ERROR:
		m_player.Close();
		m_video2D.Destory();
		return FALSE;
	}

	BOOL MPlayController::Close()
	{
		if (m_player.Close())
		{
			m_video2D.Destory();
			return TRUE;
		}
		return FALSE;
	}

	void MPlayController::OnVideo(BYTE* bits, LONG size)
	{
		TinySize videoSize = m_player.GetSize();
		if (m_video2D.Copy(m_graphics.GetDX11(), NULL, bits, size))
		{
			m_graphics.GetDX11().SetRenderTexture2D(NULL);
			m_graphics.GetDX11().GetRender2D()->BeginDraw();
			m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
			m_graphics.GetDX11().GetRender2D()->EndDraw();
			m_graphics.Present();
		}
	}
	void MPlayController::OnVolume(DWORD pos)
	{
		m_player.SetVolume(pos);
		/*if (pos == 0)
		{
			m_player.SetVolume(-10000);
		}
		else
		{
			LONG volume = static_cast<LONG>(floorf(2000.0F * log10f((FLOAT)(pos) / (FLOAT)100) + 0.5F));
			m_player.SetVolume(volume);
		}*/
	}
}
