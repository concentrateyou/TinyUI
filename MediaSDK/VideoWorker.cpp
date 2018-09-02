#include "stdafx.h"
#include "VideoWorker.h"

namespace MediaSDK
{
	VideoWorker::VideoWorker(TinyTaskManeger& manager)
		:m_manager(manager),
		m_display(m_dx11),
		m_video(m_dx11),
		m_stop(FALSE)
	{

	}

	VideoWorker::~VideoWorker()
	{

	}

	DX11& VideoWorker::GetDX11()
	{
		return m_dx11;
	}

	BOOL VideoWorker::Initialize(HWND hWND, const TinySize& canvasSize, const TinySize& outputSize)
	{
		if (!m_dx11.Initialize(hWND, canvasSize.cx, canvasSize.cy))
			return FALSE;
		if (!m_display.InitializeShaders())
			return FALSE;
		if (!m_display.Create())
			return FALSE;
		if (!m_video.InitializeShaders())
			return FALSE;
		if (!m_video.Create(outputSize.cx, outputSize.cy))
			return FALSE;
		return TRUE;
	}
	BOOL VideoWorker::Resize(const TinySize& size)
	{
		TinyAutoLock autolock(m_lock);
		m_display.Destory();
		if (!m_dx11.Resize(size.cx, size.cy))
		{
			LOG(ERROR) << "[OnSize] Resize FAIL";
			return FALSE;
		}
		if (!m_display.Create())
		{
			LOG(ERROR) << "[OnSize] Display Create Graphics FAIL";
			return FALSE;
		}
		return TRUE;
	}
	BOOL VideoWorker::Start()
	{
		m_stop = FALSE;
		m_videoTask = m_manager.PostTask(BindCallback(&VideoWorker::OnMessagePump, this), 0);
		if (!m_videoTask)
			return FALSE;
		return TRUE;
	}

	void VideoWorker::Stop()
	{
		m_stop = TRUE;
		if (m_videoTask != NULL)
		{
			m_videoTask->Close();
			m_videoTask = NULL;
		}
	}
	void VideoWorker::Add(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.Add(visual);
	}

	void VideoWorker::Remove(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.Remove(visual);
	}

	void VideoWorker::RemoveAll()
	{
		TinyAutoLock autolock(m_lock);
		m_visuals.RemoveAll();
	}

	void VideoWorker::MoveUp(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		INT index = m_visuals.Lookup(visual);
		if (index > 0)
		{
			IVisual2D* val = m_visuals[index - 1];
			m_visuals[index - 1] = visual;
			m_visuals[index] = val;
		}
	}

	void VideoWorker::MoveDown(IVisual2D* visual)
	{
		TinyAutoLock autolock(m_lock);
		INT index = m_visuals.Lookup(visual);
		if (index > 0 && index < m_visuals.GetSize())
		{
			IVisual2D* val = m_visuals[index + 1];
			m_visuals[index + 1] = visual;
			m_visuals[index] = val;
		}
	}
	IVisual2D* VideoWorker::HitTest(const TinyPoint& pos)
	{
		for (INT i = m_visuals.GetSize() - 1; i >= 0; i--)
		{
			IVisual2D* visual2D = m_visuals[i];
			TinyRectangle rectangle;
			visual2D->GetTrackerRect(&rectangle);
			if (rectangle.PtInRect(pos))
			{
				return visual2D;
			}
		}
		return NULL;
	}
	void VideoWorker::OnMessagePump()
	{
		UINT64 latestNS = 0;
		UINT64 videoNS = TinyTime::Now();
		for (;;)
		{
			if (m_stop)
				break;
			UINT64 beginNS = TinyTime::Now();
			OnRender();
			m_dx11.Present();
			Sleep(20);
		}
	}
	void VideoWorker::OnRender()
	{
		TinyAutoLock autolock(m_lock);
		m_display.BeginDraw();
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual2D* visual2D = m_visuals[i];
			visual2D->Tick();
			DX11Image2D* image2D = visual2D->GetVisual2D();
			m_display.DrawImage(*image2D);
		}
		m_display.EndDraw();
	}
}

