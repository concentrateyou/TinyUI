#pragma once
#include "DXFramework.h"
using namespace TinyUI;

struct Sample
{
	DWORD	dwType;
	DWORD	dwFlag;
	DWORD	dwIndex;
	DWORD	dwTime;
	DWORD	dwPTS;
	DWORD	dwDTS;
	DWORD	dwSize;
	BYTE*	sample;
};