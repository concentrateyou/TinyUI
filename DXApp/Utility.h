#pragma once
#include "DXFramework.h"
using namespace TinyUI;

typedef struct tagMediaTag
{
	DWORD	dwType;
	DWORD	dwFlag;
	DWORD	dwINC;
	DWORD	dwTime;
	DWORD	dwPTS;
	DWORD	dwDTS;
}MediaTag;

typedef struct tagSample
{
	MediaTag	mediaTag;
	LONG		size;
	BYTE*		bits;
}Sample;

BOOL  WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList);