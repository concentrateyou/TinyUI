#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_clock(INVALID_TIME),
		m_baseTime(INVALID_TIME)
	{
		m_event.CreateEvent();
	}

	MClock::~MClock()
	{
	}

	void MClock::SetClock(FLOAT clock)
	{
		m_clock = clock;
	}

	FLOAT MClock::GetClock()
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

