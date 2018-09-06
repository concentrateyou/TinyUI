#include "stdafx.h"
#include "VideoWorker.h"

namespace MediaSDK
{
	VideoWorker::VideoWorker(TinyTaskManeger& manager)
		:m_manager(manager),
		m_display(m_dx11),
		m_stop(FALSE),
		m_texture(0)
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
		for (INT i = 0; i < NUM_TEXTURES; i++)
		{
			m_videos[i] = new DX11Graphics2D(m_dx11);
			if (!m_videos[i]->InitializeShaders())
				return FALSE;
			if (!m_videos[i]->Create(outputSize.cx, outputSize.cy))
				return FALSE;
		}
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
			OnDisplay();
			m_dx11.Present();
			Sleep(30);
		}
	}
	void VideoWorker::OnDisplay()
	{
		TinyAutoLock autolock(m_lock);
		m_display.BeginDraw();
		for (INT i = 0; i < m_visuals.GetSize(); i++)
		{
			IVisual2D* visual2D = m_visuals[i];
			INT64 timestamp = 0;
			if (visual2D->Tick(timestamp))
			{
				visual2D->Draw(m_display);
			}
		}
		m_display.EndDraw();
	}
	void VideoWorker::OnOutputs()
	{

	}
}

