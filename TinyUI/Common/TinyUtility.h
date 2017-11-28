#pragma once
#include <Windows.h>
#include <Ntsecapi.h>
#include <algorithm>
#include <limits>
#include <string>
#include "TinyString.h"
using namespace std;

namespace TinyUI
{
	void RandBytes(void* output, size_t outputSize);
	INT RandInteger(INT min, INT max);
	ULONGLONG RandUInteger64();
	ULONGLONG RandGenerator(ULONGLONG range);
	string GenerateGUID();
	string GUIDToString(const GUID& guid);

	static inline INT FLS(INT x)
	{
		INT pos;
		INT i;
		if (0 != x)
		{
			for (i = (x >> 1), pos = 0; i != 0; ++pos)
				i >>= 1;
		}
		else
		{
			pos = -1;
		}
		return pos + 1;
	}
	static inline UINT ROUNDUP_POW_2(UINT x)
	{
		return 1UL << FLS(x - 1);
	}
}

