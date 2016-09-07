#include "stdafx.h"
#include "EncodeTask.h"

#define ENCODE_FINISH_EVENT     TEXT("ENCODE_FINISH")

EncodeTask::EncodeTask()
{
}

BOOL EncodeTask::Submit()
{
	if (m_break.CreateEvent(FALSE, FALSE, ENCODE_FINISH_EVENT))
	{
		Closure s = BindCallback(&EncodeTask::MessagePump, this);
		return TinyTask::Submit(s);
	}
	return FALSE;
}


void EncodeTask::MessagePump()
{
	m_timer.BeginTime();

	for (;;)
	{
		if (m_break.Lock(0))
		{
			break;
		}
		LONGLONG offset = m_timer.GetMicroseconds();
	}
	m_timer.EndTime();
}

EncodeTask::~EncodeTask()
{
}
