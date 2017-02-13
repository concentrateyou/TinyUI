#pragma once
#include <stdio.h>
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace Utility
{
	INT ToINT32(BYTE val[4]);
	INT ToINT24(BYTE val[3]);
	INT ToINT16(BYTE val[2]);
	INT ToINT8(BYTE val[1]);
	void WINAPI SaveBitmap(const  BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize);
}