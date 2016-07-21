#include "../stdafx.h"
#include "TinyRand.h"

namespace TinyUI
{
	ULONGLONG RandUInteger64()
	{
		ULONGLONG number;
		RandBytes(&number, sizeof(number));
		return number;
	}
	ULONGLONG RandGenerator(ULONGLONG range)
	{
		ULONGLONG max_acceptable_value = ((std::numeric_limits<ULONGLONG>::max)() / range) * range - 1;
		ULONGLONG value;
		do
		{
			value = RandUInteger64();
		} while (value > max_acceptable_value);

		return value % range;
	}
	INT RandInteger(INT min, INT max)
	{
		ULONGLONG range = static_cast<LONGLONG>(max)-min + 1;
		return static_cast<INT>(min + static_cast<LONGLONG>(RandGenerator(range)));
	}
	void RandBytes(void* output, size_t outputSize)
	{
		char* pOutput = static_cast<char*>(output);
		while (outputSize > 0)
		{
			const ULONGLONG outputBytes = static_cast<ULONG>(std::min<size_t>(outputSize, static_cast<size_t>((std::numeric_limits<ULONG>::max)())));
			const BOOL success = RtlGenRandom(pOutput, outputBytes) != FALSE;
			outputSize -= outputBytes;
			pOutput += outputBytes;
		}
	}
}
