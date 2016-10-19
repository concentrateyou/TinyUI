#include "stdafx.h"
#include "ReferenceTime.h"

namespace Media
{
	ReferenceTime::ReferenceTime()
		:m_time(0)
	{

	}
	ReferenceTime::ReferenceTime(LONG msecs)
	{
		m_time = MILLISECONDS_TO_100NS_UNITS(msecs);
	}
	ReferenceTime::ReferenceTime(REFERENCE_TIME rt)
	{
		m_time = rt;
	}
	ReferenceTime::~ReferenceTime()
	{

	}
	ReferenceTime::operator REFERENCE_TIME() const
	{
		return m_time;
	}

	ReferenceTime& ReferenceTime::operator=(const ReferenceTime& rt)
	{
		m_time = rt.m_time;
		return *this;
	};
	ReferenceTime& ReferenceTime::operator=(const LONGLONG ll)
	{
		m_time = ll;
		return *this;
	};

	ReferenceTime& ReferenceTime::operator+=(const ReferenceTime& rt)
	{
		return (*this = *this + rt);
	};

	ReferenceTime& ReferenceTime::operator-=(const ReferenceTime& rt)
	{
		return (*this = *this - rt);
	};

	LONG ReferenceTime::Millisecs()
	{
		return (LONG)(m_time / (UNITS / MILLISECONDS));
	};

	LONGLONG ReferenceTime::GetUnits()
	{
		return m_time;
	};

}

