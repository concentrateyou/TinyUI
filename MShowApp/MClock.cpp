#include "stdafx.h"
#include "MClock.h"

namespace MShow
{
	MClock::MClock()
		:m_baseTime(-1)
	{
	}

	MClock::~MClock()
	{
	}
	void MClock::SetBaseTime(LONG baseTime)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime = baseTime;
	}
	void MClock::AddBaseTime(DWORD dwMS)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime += dwMS;
	}
	LONG MClock::GetBaseTime() const
	{
		return m_baseTime;
	}
}

