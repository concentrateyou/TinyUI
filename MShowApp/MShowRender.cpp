#include "stdafx.h"
#include "MShowRender.h"

namespace MShow
{
	MShowRender::MShowRender(MReadTask& task, MClock& clock)
		:m_task(task),
		m_videoTask(m_task, clock),
		m_videoRenderTask(m_videoTask, clock),
		m_audioTask(task, clock),
		m_audioRenderTask(m_audioTask, clock)
	{

	}

	BOOL MShowRender::Initialize(HWND hWND)
	{
		BOOL bRes = m_videoRenderTask.Initialize(hWND);
		bRes |= m_audioRenderTask.Initialize(hWND);
		return bRes;
	}

	BOOL MShowRender::Submit()
	{
		BOOL bRes = m_videoTask.Submit();
		bRes |= m_videoRenderTask.Submit();
		bRes |= m_audioTask.Submit();
		bRes |= m_audioRenderTask.Submit();
		return bRes;
	}
	BOOL MShowRender::Close()
	{
		BOOL bRes = m_videoTask.Close(INFINITE);
		bRes |= m_videoRenderTask.Close(INFINITE);
		bRes |= m_audioTask.Close(INFINITE);
		bRes |= m_audioRenderTask.Close(INFINITE);
		return bRes;
	}

	MShowRender::~MShowRender()
	{

	}
}