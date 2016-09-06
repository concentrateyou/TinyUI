#include "stdafx.h"
#include "PublishTask.h"
#include "GraphicsCapture.h"

#define PUBLISH_FINISH_EVENT     TEXT("PUBLISH_FINISH")

PublishTask::PublishTask(GraphicsCapture* pSys)
	:m_pSys(pSys)
{
}

BOOL PublishTask::Submit()
{
	if (!m_publish.OpenEvent(EVENT_ALL_ACCESS, FALSE, PUBLISH_FINISH_EVENT) &&
		!m_publish.CreateEvent(FALSE, FALSE, PUBLISH_FINISH_EVENT))
	{
		return FALSE;
	}
	Closure s = BindCallback(&PublishTask::MessagePump, this);
	return TinyTask::Submit(s);
}


void PublishTask::MessagePump()
{
	for (;;)
	{
		if (m_publish && m_publish.Lock(3))
		{
			break;
		}
		if (m_pSys)
		{
			m_pSys->Publish();
		}
	}
}

PublishTask::~PublishTask()
{
}
