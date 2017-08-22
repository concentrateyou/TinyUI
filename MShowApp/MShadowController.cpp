#include "stdafx.h"
#include "MShadowController.h"
#include "MShowApp.h"

namespace MShow
{
	MShadowController::MShadowController(MShadowView& view, MClock& clock)
		:m_view(view),
		m_bBreak(FALSE),
		m_videoFPS(25),
		m_clock(clock),
		m_renderView(m_graphics.GetDX11())
	{
		m_signal.CreateEvent();
	}

	MShadowController::~MShadowController()
	{

	}
	BOOL MShadowController::Initialize()
	{
		TinyRectangle s;
		m_view.GetClientRect(&s);
		if (!m_graphics.Initialize(m_view.Handle(), s.Size()))
			return FALSE;
		return TRUE;
	}
	BOOL MShadowController::SetPulgSize(const TinySize& size)
	{
		MPreviewController* pCTRL = MShowApp::GetInstance().GetController().GetPreviewController();
		if (!pCTRL)
			return FALSE;
		m_pulgSize = size;
		m_image2D.Destory();
		if (!m_image2D.Load(m_graphics.GetDX11(), pCTRL->GetRenderView().GetHandle()))
			return FALSE;
		if (!m_renderView.Create(static_cast<INT>(m_pulgSize.cx), static_cast<INT>(m_pulgSize.cy), TRUE))
			return FALSE;
		return TRUE;
	}

	TinySize MShadowController::GetPulgSize() const
	{
		return m_pulgSize;
	}

	void MShadowController::SetVideoFPS(INT	videoFPS)
	{
		m_videoFPS = videoFPS;
	}

	INT	MShadowController::GetVideoFPS() const
	{
		return m_videoFPS;
	}

	MShadowView& MShadowController::GetView()
	{
		return m_view;
	}

	MClock&	MShadowController::GetClock()
	{
		return m_clock;
	}

	TinyEvent&	MShadowController::GetSignal()
	{
		return m_signal;
	}

	MPacketAllocQueue&	MShadowController::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MShadowController::OnMessagePump()
	{
		SampleTag sampleTag;
		for (;;)
		{
			if (m_bBreak)
				break;
			while (m_clock.GetBaseTime() == -1);
			TinyEvent& signal = MShow::MShowApp::GetInstance().GetController().GetPreviewController()->GetSignal();
			if (signal.Lock(1000))
			{
				ZeroMemory(&sampleTag, sizeof(sampleTag));
				DWORD dwTime = OnVideo(sampleTag);
				TRACE("dwTime:%d\n", dwTime);
				m_clock.SetVideoPTS(MShow::MShowApp::GetInstance().GetQPCTimeMS());//设置视频流时间
				sampleTag.timestamp = m_clock.GetVideoPTS() - m_clock.GetBaseTime();
				m_videoQueue.Push(sampleTag);
				m_signal.SetEvent();
			}
		}
	}

	DWORD MShadowController::OnVideo(SampleTag& sampleTag)
	{
		m_timeQPC.BeginTime();
		m_graphics.GetDX11().SetRenderTexture2D(&m_renderView);
		m_graphics.GetDX11().GetRender2D()->BeginDraw();
		m_graphics.GetDX11().AllowBlend(FALSE, NULL);
		m_graphics.GetDX11().AllowDepth(TRUE);
		m_graphics.DrawImage(&m_image2D, (FLOAT)((FLOAT)m_pulgSize.cx / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cx)), (FLOAT)((FLOAT)m_pulgSize.cy / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cy)));
		m_graphics.GetDX11().GetRender2D()->EndDraw();
		DWORD dwSize = 0;
		BYTE* bits = m_renderView.Map(dwSize);
		if (bits != NULL && dwSize > 0)
		{
			sampleTag.size = dwSize;
			if (m_videoQueue.GetAllocSize() == 0)
			{
				INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
				m_videoQueue.Initialize(count, sampleTag.size + 4);
			}
			sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
			memcpy_s(sampleTag.bits + 4, sampleTag.size, bits, sampleTag.size);
		}
		m_renderView.Unmap();
		m_timeQPC.EndTime();
		return static_cast<DWORD>(m_timeQPC.GetMillisconds());
	}
	BOOL MShadowController::Submit()
	{
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MShadowController::OnMessagePump, this));
	}
	BOOL MShadowController::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_image2D.Destory();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;

	}
}