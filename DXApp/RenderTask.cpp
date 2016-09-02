#include "stdafx.h"
#include "RenderTask.h"
#include "GraphicsCapture.h"

#define RENDER_FINISH_EVENT     TEXT("RENDER_FINISH")

RenderTask::RenderTask(GraphicsCapture* pSys, TinyWin32TaskPool* pWorks)
	:TinyWin32Task(pWorks),
	m_pSys(pSys)
{

}


RenderTask::~RenderTask()
{
}

BOOL RenderTask::Submit()
{
	if (!m_render.OpenEvent(EVENT_ALL_ACCESS, FALSE, RENDER_FINISH_EVENT) &&
		!m_render.CreateEvent(FALSE, FALSE, RENDER_FINISH_EVENT))
	{
		return FALSE;
	}
	Closure s = BindCallback(&RenderTask::MessagePump, this);
	return TinyWin32Task::Submit(s);
}
void RenderTask::MessagePump()
{
	for (;;)
	{
		Sleep(3);
		if (m_render && m_render.Lock(20))
		{
			break;
		}
		if (m_pSys)
		{
			m_pSys->Render();
		}
	}
}