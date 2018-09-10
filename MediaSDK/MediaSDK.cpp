#include "stdafx.h"
#include "MediaSDK.h"

namespace MediaSDK
{
	BOOL WINAPI SleepNS(UINT64 ns)
	{
		UINT64 _now = TinyTime::Now();
		UINT32 ms;
		if (_now >= ns)
			return FALSE;
		ms = (uint32_t)((ns - _now) / 1000000);
		if (ms > 1)
		{
			Sleep(ms - 1);
		}
		for (;;)
		{
			_now = TinyTime::Now();
			if (_now >= ns)
				return TRUE;
			Sleep(0);
		}
	}
}