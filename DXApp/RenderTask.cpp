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
	if (m_break.CreateEvent(FALSE, FALSE, RENDER_FINISH_EVENT) != NULL)
	{
		Closure s = BindCallback(&RenderTask::MessagePump, this);
		return  TinyTask::Submit(s);
	}
	return FALSE;
}
void RenderTask::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
		{
			break;
		}
		if (m_pSys)
		{
			m_pSys->Render();
		}
		
	}
}