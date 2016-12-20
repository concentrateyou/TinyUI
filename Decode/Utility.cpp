#include "stdafx.h"
#include "Utility.h"

namespace Utility
{
	INT ToINT32(BYTE val[4])
	{
		return (INT)(((val[0] & 0xFF) << 24) |
			((val[1] & 0xFF) << 16) |
			((val[2] & 0xFF) << 8) |
			((val[3] & 0xFF)));
	}
	INT ToINT24(BYTE val[3])
	{
		return (INT)(((val[0] & 0xFF) << 16) |
			((val[1] & 0xFF) << 8) |
			((val[2] & 0xFF)));
	}
}