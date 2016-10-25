#pragma once
#include "DXFramework.h"
using namespace TinyUI;

class Sample
{
public:
	DWORD	dwType;
	DWORD	dwFlag;
	DWORD	dwINC;
	DWORD	dwTime;
	DWORD	dwPTS;
	DWORD	dwDTS;
	DWORD	dwSize;
	BYTE*	bits;
};