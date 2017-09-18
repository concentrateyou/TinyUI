#include "stdafx.h"
#include "MShadowController.h"
#include "MPreviewController.h"
#include "MShowApp.h"

namespace MShow
{
	MShadowController::MShadowController(MShadowView& view, MClock& clock)
		:m_view(view),
		m_videoFPS(25),
		m_clock(clock)
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
		BYTE* bits = NULL;
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, bits, TRUE))
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

	MPacketAllocQueue&	MShadowController::GetVideoQueue()
	{
		return m_videoQueue;
	}

	DWORD MShadowController::OnVideo(SampleTag& sampleTag)
	{
		m_timeQPC.BeginTime();
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		MShow::MShowApp::GetInstance().GetController().GetPreviewController()->Graphics().Enter();
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
		MShow::MShowApp::GetInstance().GetController().GetPreviewController()->Graphics().Leave();
		m_timeQPC.EndTime();
		return static_cast<DWORD>(m_timeQPC.GetMillisconds());
	}

	void MShadowController::OnTimer()
	{
		while (m_clock.GetBaseTime() == -1);
		m_clock.SetVideoPTS(MShow::MShowApp::GetInstance().GetQPCTimeMS());//设置视频流时间
		SampleTag sampleTag;
		OnVideo(sampleTag);
		sampleTag.timestamp = m_clock.GetVideoPTS() - m_clock.GetBaseTime();
		m_videoQueue.Push(sampleTag);
	}
	BOOL MShadowController::Start()
	{
		DWORD delay = 1000 / m_videoFPS;
		return m_timer.SetCallback(delay, BindCallback(&MShadowController::OnTimer, this));
	}
	void MShadowController::Stop()
	{
		m_timer.Close();
		m_videoQueue.RemoveAll();
	}
	BOOL MShadowController::Uninitialize()
	{
		m_image2D.Destory();
		m_copy2D.Destory();
		m_videoQueue.RemoveAll();
		return TRUE;
	}
}