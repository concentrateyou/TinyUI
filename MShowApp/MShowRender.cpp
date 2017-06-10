#include "stdafx.h"
#include "MShowRender.h"

namespace MShow
{
	MShowRender::MShowRender(MReadTask& task, MClock& clock)
		:m_task(task),
		m_videoTask(m_task, clock),
		m_renderTask(m_videoTask, clock)
	{

	}

	BOOL MShowRender::Initialize(HWND hWND)
	{
		return m_renderTask.Initialize(hWND);
	}

	BOOL MShowRender::Submit()
	{
		BOOL bRes = m_videoTask.Submit();
		bRes |= m_renderTask.Submit();
		return bRes;
	}
	BOOL MShowRender::Close()
	{
		BOOL bRes = m_videoTask.Close(INFINITE);
		bRes = m_renderTask.Close(INFINITE);
		return bRes;
	}

	MShowRender::~MShowRender()
	{

	}
}