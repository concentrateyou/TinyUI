#pragma once
#include "Render/TinyGDI.h"
#include "DXFramework.h"
#include <Richedit.h>
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

BOOL WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList);
BOOL WINAPI ScreenSave(const TinyRectangle& s);
BOOL WINAPI InvertWindow(HWND hWND);
Gdiplus::RectF WINAPI MeasureString(HDC hDC, const wstring& str, const CHARFORMAT& cf);