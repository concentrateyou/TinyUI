#include "stdafx.h"
#include "BaseWorker.h"


BaseWorker::BaseWorker(Closure& callback)
	:m_callback(callback)
{
	string str = GenerateGUID();
	if (!m_break.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
		throw("create event error!");
}


BaseWorker::~BaseWorker()
{
}

BOOL BaseWorker::Submit()
{
	Closure s = BindCallback(&BaseWorker::MessagePump, this);
	return TinyTask::Submit(s);
}

BOOL BaseWorker::Quit()
{
	m_break.SetEvent();
}

void BaseWorker::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
			break;
		if (!m_callback.IsNull())
		{
			m_callback();
		}
	}
}
