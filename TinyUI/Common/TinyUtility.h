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
}

