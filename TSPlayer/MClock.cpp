#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_audioPTS(INVALID_TIME),
		m_baseTime(INVALID_TIME)
	{
		m_event.CreateEvent();
	}

	MClock::~MClock()
	{
	}
	void MClock::SetAudioPTS(LONGLONG audioPTS)
	{
		m_audioPTS = audioPTS;
	}

	void MClock::SetBaseTime(LONGLONG baseTime)
	{
		m_baseTime = baseTime;
	}

	LONGLONG MClock::GetBaseTime()
	{
		return m_baseTime;
	}

	LONGLONG MClock::GetAudioPTS()
	{
		return m_audioPTS;
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

