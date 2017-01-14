#include "../stdafx.h"
#include "TinyUtility.h"
#include "TinyCore.h"
#include <objbase.h>

namespace TinyUI
{
	typedef BOOL(WINAPI *pfnSystemFunction036)(PVOID, ULONG);
	void RandBytes(void* output, size_t outputSize)
	{
		TinyScopedLibrary advapi32(TEXT("advapi32.dll"));
		pfnSystemFunction036 systemFunction036 = (pfnSystemFunction036)::GetProcAddress(advapi32, TEXT("SystemFunction036"));
		char* pOutput = static_cast<char*>(output);
		while (outputSize > 0)
		{
			const ULONG outputBytes = static_cast<ULONG>(std::min<size_t>(outputSize, static_cast<size_t>((std::numeric_limits<ULONG>::max)())));
			const BOOL success = systemFunction036(pOutput, outputBytes) != FALSE;
			outputSize -= outputBytes;
			pOutput += outputBytes;
		}
	}
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
		ULONGLONG range = static_cast<LONGLONG>(max) - min + 1;
		return static_cast<INT>(min + static_cast<LONGLONG>(RandGenerator(range)));
	}
	string RandomDataToGUIDString(const ULONGLONG bytes[2])
	{
		return StringPrintf("%08x-%04x-%04x-%04x-%012llx",
			static_cast<unsigned int>(bytes[0] >> 32),
			static_cast<unsigned int>((bytes[0] >> 16) & 0x0000ffff),
			static_cast<unsigned int>(bytes[0] & 0x0000ffff),
			static_cast<unsigned int>(bytes[1] >> 48),
			bytes[1] & 0x0000ffffffffffffULL);
	}
	string GenerateGUID()
	{
		const INT GUIDSize = 39;
		GUID guid;
		if (SUCCEEDED(CoCreateGuid(&guid)))
		{
			wstring val;
			val.resize(GUIDSize);
			if (StringFromGUID2(guid, &val[0], GUIDSize) == GUIDSize)
			{
				return WStringToString(val);
			}
		}
		return string();
	}

}
