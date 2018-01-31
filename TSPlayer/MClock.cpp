#include "stdafx.h"
#include "MClock.h"

namespace TSPlayer
{
	MClock::MClock()
		:m_baseTime(INVALID_TIME),
		m_basePTS(INVALID_TIME)
	{
	}

	MClock::~MClock()
	{
	}
	void MClock::SetBaseTime(LONGLONG baseTime)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime = baseTime;
	}
	void MClock::AddBaseTime(LONGLONG dwMS)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime += dwMS;
	}
	LONGLONG MClock::GetBaseTime() const
	{
		return m_baseTime;
	}
	LONGLONG MClock::GetBasePTS() const
	{
		return m_basePTS;
	}
	void MClock::SetBasePTS(LONGLONG basePTS)
	{
		m_basePTS = basePTS;
	}
}

