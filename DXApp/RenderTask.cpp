#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask(DXWindow* window)
	:m_window(window)
{
	string str = GenerateGUID();
	if (!m_event.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
		throw("create event error!");
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
	m_event.SetEvent();
}

void RenderTask::MessagePump()
{
	for (;;)
	{
		if (m_event.Lock(20))
			break;
		m_window->Render();
	}
}
