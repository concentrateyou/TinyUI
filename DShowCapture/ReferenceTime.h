#pragma once
#include "DShowCommon.h"

const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
const LONGLONG UNITS = (NANOSECONDS / 100);      // 10 ^ 7

#define MILLISECONDS_TO_100NS_UNITS(lMs) \
    Int32x32To64((lMs), (UNITS / MILLISECONDS))

namespace DShow
{
	class ReferenceTime
	{
	public:
		ReferenceTime();
		ReferenceTime(LONG msecs);
		ReferenceTime(REFERENCE_TIME rt);
		~ReferenceTime();
		operator REFERENCE_TIME() const;
		ReferenceTime& operator=(const ReferenceTime& rt);
		ReferenceTime& operator=(const LONGLONG ll);
		ReferenceTime& operator+=(const ReferenceTime& rt);
		ReferenceTime& operator-=(const ReferenceTime& rt);
		LONG Millisecs();
		LONGLONG GetUnits();
	protected:
		REFERENCE_TIME m_time;
	};
}

