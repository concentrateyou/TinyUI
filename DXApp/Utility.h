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

class GraphicsObserver : public TinyLock
{
	DISALLOW_COPY_AND_ASSIGN(GraphicsObserver)
public:
	GraphicsObserver();
public:
	virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) = 0;
protected:
	virtual ~GraphicsObserver();
};

BOOL WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList);
BOOL WINAPI ScreenSave(const TinyRectangle& s);
BOOL WINAPI WindowSave(HWND hWND);
BOOL WINAPI InvertWindow(HWND hWND);