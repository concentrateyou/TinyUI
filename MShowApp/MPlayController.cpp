#include "stdafx.h"
#include "MPlayController.h"
#include "MShowApp.h"
#include "MPreviewController.h"

namespace MShow
{
	MPlayController::MPlayController(MPlayView& view)
		:m_view(view),
		m_bBreak(FALSE)
	{

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
		if (!m_video2D.Load(m_graphics.GetDX11(), MShow::MShowApp::GetInstance().GetController().GetPreviewController()->GetHandle()))
			return FALSE;
		return TRUE;
	}

	BOOL MPlayController::Submit()
	{
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MPlayController::OnMessagePump, this));
	}

	BOOL MPlayController::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_video2D.Destory();
			return TRUE;
		}
		return FALSE;
	}

	void MPlayController::OnMessagePump()
	{
		TinyPerformanceTimer timeQPC;
		for (;;)
		{
			if (m_bBreak)
				break;
			BOOL bRes = MShow::MShowApp::GetInstance().GetController().GetPreviewController()->Lock(INFINITE);
			if (bRes)
			{
				m_video2D.Lock(0, 250);
				m_graphics.GetDX11().SetRenderTexture2D(NULL);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(FALSE, blendFactor);
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
				m_graphics.Present();
				m_video2D.Unlock(0);
			}
		}
	}
}
