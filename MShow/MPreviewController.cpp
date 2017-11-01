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
		:m_view(view)
	{

	}

	MPreviewController::~MPreviewController()
	{

	}

	BOOL MPreviewController::Initialize()
	{
		TinyRectangle rectangle;
		GetClientRect(m_view.Handle(), &rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
		{
			LOG(ERROR) << "[MPreviewController] " << "DX11Graphics2D Initialize FAIL";
			return FALSE;
		}
		LOG(INFO) << "[MPreviewController] " << "DX11Graphics2D Initialize OK";
		return TRUE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		TinySize size;
		TinyRectangle rectangle;
		GetClientRect(m_view.Handle(), &rectangle);
		m_player.Reset(new MFLVPlayer(BindCallback(&MPreviewController::OnAudio, this), BindCallback(&MPreviewController::OnVideoCopy, this), BindCallback(&MPreviewController::OnVideoRender, this)));
		if (!m_player)
			goto _ERROR;
		if (!m_player->Open(m_view.Handle(), pzURL))
		{
			LOG(ERROR) << "[MPreviewController] " << "Open FAIL";
			goto _ERROR;
		}
		LOG(INFO) << "[MPreviewController] " << "Player Open OK";
		size = m_player->GetSize();
		m_video2D.Destory();
		if (!m_video2D.Create(m_graphics.GetDX11(), size.cx, size.cy, FALSE))
		{
			LOG(ERROR) << "[MPreviewController] " << "Video2D Create FAIL";
			goto _ERROR;
		}
		LOG(INFO) << "[MPreviewController] " << "Video2D Create OK";
		m_video2D.SetScale(rectangle.Size());
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL MPreviewController::Close()
	{
		if (m_player != NULL)
		{
			m_player->Close();
		}
		m_video2D.Destory();
		LOG(INFO) << "[MPreviewController] " << "Player Close OK";
		return TRUE;
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}
	LONGLONG MPreviewController::GetBasePTS()
	{
		ASSERT(m_player);
		return m_player->GetBasePTS();
	}
	void MPreviewController::OnAudio(BYTE* bits, LONG size)
	{

	}

	void MPreviewController::OnVideoCopy(BYTE* bits, LONG size)
	{
		if (bits != NULL && size > 0)
		{
			TinySize videoSize = m_player->GetSize();
			if (m_video2D.Copy(m_graphics.GetDX11(), NULL, bits, size))
			{
				m_graphics.GetDX11().SetRenderTexture2D(NULL);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(FALSE, blendFactor);
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
			}
		}
	}
	void MPreviewController::OnVideoRender()
	{
		m_graphics.Present();
	}
}
