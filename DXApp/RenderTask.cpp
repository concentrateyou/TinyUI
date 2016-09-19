#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask(DXWindow* window, INT frameRate)
	:m_window(window),
	m_frameRate(frameRate)
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
		INT s = 1000 / m_frameRate;
		if (m_event.Lock(s))
			break;
		m_window->Render();
	}
}
