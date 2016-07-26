#include "../stdafx.h"
#include "TinyRand.h"
#include "TinyCore.h"

namespace TinyUI
{
	typedef BOOL(WINAPI *SystemFunction036_T)(PVOID, ULONG);
	SystemFunction036_T SystemFunction036;

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
		TinyScopedLibrary advapi32(TEXT("advapi32.dll"));
		SystemFunction036 = (SystemFunction036_T)GetProcAddress(advapi32, "SystemFunction036");
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
