#include "stdafx.h"
#include "MShadowController.h"
#include "MShowApp.h"

namespace MShow
{
	MShadowController::MShadowController(MShadowView& view)
		:m_view(view),
		m_bBreak(FALSE),
		m_videoFPS(25)
	{

	}

	MShadowController::~MShadowController()
	{

	}
	BOOL MShadowController::Initialize()
	{
		TinyRectangle s;
		m_view.GetClientRect(&s);
		if (!m_dx11.Initialize(m_view.Handle(), s.Size().cx, s.Size().cy))
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
		if (!m_image2D.Load(m_dx11, pCTRL->GetRenderView().GetHandle()))
			return FALSE;
		m_copy2D.Destory();
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, NULL, TRUE))
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
	MPacketAllocQueue&	MShadowController::GetVideoQueue()
	{
		return m_videoQueue;
	}
	void MShadowController::OnMessagePump()
	{
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		SampleTag tag;
		for (;;)
		{
			if (m_bBreak)
				break;
			Sleep(100);
			/*DWORD dwTime = OnVideo(tag);
			if (tag.size > 0)
			{
				if (m_signal.Lock(INFINITE))
				{
					m_timer.EndTime();
					m_timer.BeginTime();
					m_videoQueue.Push(tag);
					INT ms = static_cast<DWORD>(m_timer.GetMillisconds()) - dwMS;
					if (ms > 0)
					{
						TRACE("ms:%d\n", ms);
					}
				}
			}*/
		}
	}

	DWORD MShadowController::OnVideo(SampleTag& sampleTag)
	{
		m_timer.BeginTime();
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		m_copy2D.Copy(m_dx11, m_image2D);
		BYTE* bits = NULL;
		UINT pitch = 0;
		if (m_copy2D.Map(m_dx11, bits, pitch, TRUE))
		{
			sampleTag.size = pitch * m_pulgSize.cy;
			if (m_videoQueue.GetAllocSize() == 0)
			{
				INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
				m_videoQueue.Initialize(count, sampleTag.size + 4);
			}
			sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
			memcpy_s(sampleTag.bits + 4, sampleTag.size, bits, sampleTag.size);
			m_copy2D.Unmap(m_dx11);
		}
		m_timer.EndTime();
		return static_cast<DWORD>(m_timer.GetMillisconds());
	}
	VOID CALLBACK MShadowController::TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
	{
		MShadowController* pThis = reinterpret_cast<MShadowController*>(lpParameter);
		if (pThis != NULL)
		{
			pThis->m_timer.EndTime();
			DWORD dwTime = static_cast<DWORD>(pThis->m_timer.GetMillisconds());
			if (dwTime > 40)
			{
				TRACE("Time:%d\n", dwTime);
			}
			pThis->m_timer.BeginTime();
		}
	}
	BOOL MShadowController::Submit()
	{
		m_bBreak = FALSE;
		m_signal.CreateEvent();
		if (m_handle != NULL)
		{
			TinyApplication::GetInstance()->GetTimers().Unregister(m_handle);
			m_handle = NULL;
		}
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		m_handle = TinyApplication::GetInstance()->GetTimers().Register(&MShadowController::TimerCallback, this, dwMS - 1, dwMS - 1, WT_EXECUTEINTIMERTHREAD);
		return TinyTaskBase::Submit(BindCallback(&MShadowController::OnMessagePump, this));
	}
	BOOL MShadowController::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		m_signal.Close();
		if (TinyTaskBase::Close(dwMS))
		{
			m_image2D.Destory();
			m_copy2D.Destory();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;

	}
}