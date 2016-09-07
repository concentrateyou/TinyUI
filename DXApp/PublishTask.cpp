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
	if (m_break.CreateEvent(FALSE, FALSE, PUBLISH_FINISH_EVENT))
	{
		Closure s = BindCallback(&PublishTask::MessagePump, this);
		return TinyTask::Submit(s);
	}
	return FALSE;
}


void PublishTask::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
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
