#include "stdafx.h"
#include "Task.h"


Task::Task(Closure& invoke)
	:m_invoke(invoke)
{
	string str = GenerateGUID();
	if (!m_break.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
		throw("create event error!");
}

Task::Task(Closure&& invoke)
	:m_invoke(std::move(invoke))
{
	string str = GenerateGUID();
	if (!m_break.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
		throw("create event error!");
}

Task::~Task()
{
}

BOOL Task::Submit()
{
	Closure s = BindCallback(&Task::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void Task::Quit()
{
	m_break.SetEvent();
}

void Task::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
			break;
		if (!m_invoke.IsNull())
		{
			m_invoke();
		}
	}
}
