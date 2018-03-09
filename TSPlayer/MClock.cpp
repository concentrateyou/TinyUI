#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_audioClock(INVALID_TIME),
		m_baseTime(INVALID_TIME)
	{
		m_event.CreateEvent();
	}

	MClock::~MClock()
	{
	}

	void MClock::SetAudioClock(LONGLONG audioClock)
	{
		m_audioClock = audioClock;
	}

	LONGLONG MClock::GetAudioClock()
	{
		return m_audioClock;
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

