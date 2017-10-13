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
		return m_renderView.Create(static_cast<INT>(m_pulgSize.cx), static_cast<INT>(m_pulgSize.cy), TRUE, TRUE);
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
		/*if (!m_video2D.Load(m_graphics.GetDX11(), MShow::MShowApp::GetInstance().GetController().GetPreviewController()->GetHandle()))
			return FALSE;*/
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
			//m_video2D.Destory();
			return TRUE;
		}
		return FALSE;
	}

	void MPlayController::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			MVideoController* pCTRL = MShow::MShowApp::GetInstance().GetController().GetCurrentCTRL();
			if (pCTRL != NULL)
			{
				while (m_clock.GetBaseTime() == -1);
				m_clock.SetVideoPTS(MShow::MShowApp::GetInstance().GetQPCTimeMS());//设置视频流时间
				if (pCTRL->m_signal.Lock(1000))
				{
					/*MPreviewController* pCTRLP = MShow::MShowApp::GetInstance().GetController().GetPreviewController();
					SampleTag sampleTag;
					ZeroMemory(&sampleTag, sizeof(sampleTag));
					BYTE* ps = pCTRLP->GetPointer();
					sampleTag.size = pCTRLP->GetSize();
					if (sampleTag.size > 0)
					{
						if (m_videoQueue.GetAllocSize() == 0)
						{
							INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
							m_videoQueue.Initialize(count, sampleTag.size + 4);
						}
						sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
						memcpy_s(sampleTag.bits + 4, sampleTag.size, ps, sampleTag.size);
						m_videoQueue.Push(sampleTag);
					}*/
				}
			}
		}
	}
}
