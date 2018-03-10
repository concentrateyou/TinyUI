#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_clock(NAN),
		m_baseTime(INVALID_TIME)
	{
		m_event.CreateEvent();
	}

	MClock::~MClock()
	{
	}

	void MClock::SetClock(DOUBLE clock)
	{
		m_clock = clock;
	}

	DOUBLE MClock::GetClock()
	{
		return m_clock;
	}

	void MClock::SetBaseTime(LONGLONG baseTime)
	{
		m_baseTime = baseTime;
	}

	LONGLONG MClock::GetBaseTime()
	{
		return m_baseTime;
	}

	BOOL MClock::Sleep(DWORD dwMS)
	{
		return m_event.Lock(dwMS);
	}
	void MClock::Wake()
	{
		m_event.SetEvent();
	}
}

