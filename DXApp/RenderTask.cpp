#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask(DXWindow* window, DWORD dwFPS)
	:m_window(window),
	m_dwFPS(dwFPS)
{

}

RenderTask::~RenderTask()
{
}

BOOL RenderTask::Submit()
{
	Closure s = BindCallback(&RenderTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void RenderTask::Exit()
{
	m_signal.SetEvent();
}

void RenderTask::MessagePump()
{
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		if (m_signal.Lock(s))
			break;
		m_window->Render();
	}
}
