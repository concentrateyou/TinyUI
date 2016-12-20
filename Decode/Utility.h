#pragma once
#include <stdio.h>
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
using namespace TinyUI;

namespace Utility
{
	INT ToINT32(BYTE val[4]);
	INT ToINT24(BYTE val[3]);
}