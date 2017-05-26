#include "stdafx.h"
#include "Common.h"

namespace Decode
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
	INT ToINT16(BYTE val[2])
	{
		return	(INT)(val[0] << 8) | (val[1]);
	}
	INT ToINT8(BYTE val[1])
	{
		return	(INT)(val[0] << 8);
	}

	/*BYTE ToBYTE4(INT s)
	{
		BYTE val[4]
		val[0] = (s >> 24) & 0xFF;
		val[1] = (s >> 16) & 0xFF;
		val[2] = (s >> 8) & 0xFF;
		val[3] = s & 0xFF;
	}

	void ToBYTE3(INT s, BYTE val[3])
	{
		val[0] = (s >> 16) & 0xFF;
		val[1] = (s >> 8) & 0xFF;
		val[2] = s & 0xFF;
	}
	void ToBYTE2(INT s, BYTE val[2])
	{
		val[0] = (s >> 8) & 0xFF;
		val[1] = s & 0xFF;
	}
	void ToBYTE(INT s, BYTE val[1])
	{
		val[0] = s & 0xFF;
	}*/
}