#pragma once
#include <Windows.h>
#include <Ntsecapi.h>
#include <algorithm>
#include <limits>
#include <string>
using namespace std;

namespace TinyUI
{
	INT RandInteger(INT min, INT max);
	ULONGLONG RandUInteger64();
	void RandBytes(void* output, size_t outputSize);
}

