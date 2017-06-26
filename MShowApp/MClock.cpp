#include "stdafx.h"
#include "MClock.h"

namespace MShow
{
	MClock::MClock()
		:m_baseTime(-1),
		m_basePTS(-1)
	{
	}

	MClock::~MClock()
	{
	}
	void MClock::SetBaseTime(DWORD baseTime)
	{
		TinyAutoLock lock(m_lock);
		m_baseTime = baseTime;
	}
	void MClock::AddBaseTime(DWORD dwMS)
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

