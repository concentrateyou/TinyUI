#include "stdafx.h"
#include "MPlayController.h"
#include "MShowApp.h"
#include "MPreviewController.h"

namespace MShow
{
	MPlayController::MPlayController(MPlayView& view, MClock& clock)
		:m_view(view),
		m_clock(clock),
		m_renderView(m_graphics.GetDX11()),
		m_bBreak(FALSE)
	{

	}

	MPlayController::~MPlayController()
	{

	}
	BOOL MPlayController::SetPulgSize(const TinySize& size)
	{
		m_pulgSize = size;
		return m_renderView.Create(static_cast<INT>(m_pulgSize.cx), static_cast<INT>(m_pulgSize.cy), TRUE, FALSE);
	}

	TinySize MPlayController::GetPulgSize() const
	{
		return m_pulgSize;
	}

	void MPlayController::SetVideoFPS(INT	videoFPS)
	{
		m_videoFPS = videoFPS;
	}

	INT	MPlayController::GetVideoFPS() const
	{
		return m_videoFPS;
	}

	MClock&	MPlayController::GetClock()
	{
		return m_clock;
	}

	MPacketAllocQueue&	MPlayController::GetVideoQueue()
	{
		return m_videoQueue;
	}


	BOOL MPlayController::Initialize()
	{
		TinyRectangle rectangle;
		m_view.GetClientRect(&rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size(), TRUE))
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
		Sleep(100);
		if (TinyTaskBase::Close(dwMS))
		{
			m_video2D.Destory();
			return TRUE;
		}
		return FALSE;
	}

	BOOL MPlayController::StartPush()
	{
		m_timer.SetCallback(1000 / m_videoFPS, BindCallback(&MPlayController::OnTimer, this));
		return TRUE;
	}

	BOOL MPlayController::StopPush()
	{
		m_timer.Close();
		Sleep(100);
		return TRUE;
	}

	void MPlayController::OnTimer()
	{
		while (m_clock.GetBaseTime() == -1);
		m_clock.SetVideoPTS(MShow::MShowApp::GetInstance().GetQPCTimeMS());//设置视频流时间
		SampleTag sampleTag;
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		m_graphics.Enter();
		DWORD dwSize = 0;
		BYTE* bits = m_renderView.Map(dwSize);
		if (bits != NULL)
		{
			sampleTag.size = dwSize;
			if (m_videoQueue.GetAllocSize() == 0)
			{
				INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
				m_videoQueue.Initialize(count, sampleTag.size + 4);
			}
			sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
			memcpy_s(sampleTag.bits + 4, sampleTag.size, bits, sampleTag.size);
			m_renderView.Unmap();
			sampleTag.timestamp = m_clock.GetVideoPTS() - m_clock.GetBaseTime();
			m_videoQueue.Push(sampleTag);

		}
		m_graphics.Leave();

	}

	void MPlayController::OnMessagePump()
	{

		for (;;)
		{
			if (m_bBreak)
				break;
			BOOL bRes = MShow::MShowApp::GetInstance().GetController().GetPreviewController()->Lock(INFINITE);
			if (bRes)
			{
				m_graphics.Enter();
				m_graphics.GetDX11().SetRenderTexture2D(&m_renderView);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				m_video2D.Lock(0, 250);
				m_graphics.DrawImage(&m_video2D, (FLOAT)((FLOAT)m_pulgSize.cx / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cx)), (FLOAT)((FLOAT)m_pulgSize.cy / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cy)));
				m_video2D.Unlock(0);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
				m_graphics.GetDX11().SetRenderTexture2D(NULL);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(FALSE, blendFactor);
				m_video2D.Lock(0, 250);
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_video2D.Unlock(0);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
				m_graphics.Present();
				m_graphics.Leave();
			}
		}
	}
}
