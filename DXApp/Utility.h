#pragma once
#include "Render/TinyGDI.h"
#include "DXFramework.h"
#include <Richedit.h>
using namespace TinyUI;

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
void WINAPI SaveBitmapToFile(const  BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize);
BOOL WINAPI SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName);
BOOL WINAPI InvertWindow(HWND hWND);