#include "stdafx.h"
#include "RenderTask.h"
#include "GraphicsCapture.h"

#define RENDER_FINISH_EVENT     TEXT("RENDER_FINISH")

RenderTask::RenderTask(GraphicsCapture* pSys)
	:m_pSys(pSys)
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
	return  TinyTask::Submit(s);
}
void RenderTask::MessagePump()
{
	for (;;)
	{
		if (m_render && m_render.Lock(25))
		{
			break;
		}
		if (m_pSys)
		{
			m_pSys->Render();
		}
	}
}