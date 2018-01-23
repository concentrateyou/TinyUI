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
	BOOL FindStartCode(BYTE* bits, INT size)
	{
		for (INT i = 0;i < size - 1;i++)
		{
			if (bits[i] != 0)
				return FALSE;
		}
		if (bits[size - 1] != 1)
			return FALSE;
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL TS_PACKET_PROGRAM::operator == (const TS_PACKET_PROGRAM& other)
	{
		return ProgramPID == other.ProgramPID;
	}
}