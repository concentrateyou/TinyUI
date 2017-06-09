#include "stdafx.h"
#include "MClock.h"

namespace MShow
{
	MClock::MClock()
		:m_timestamp(-1)
	{
	}

	MClock::~MClock()
	{
	}
	void MClock::SetBaseTime(LONG timestamp)
	{
		m_timestamp = timestamp;
	}
	LONG MClock::GetBaseTime() const
	{
		return m_timestamp;
	}
}

