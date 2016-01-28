#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyGDI.h"
#include "TinyUtility.h"

namespace TinyUI
{
	namespace Windowless
	{
		typedef struct tagVIS
		{
			struct tagVIS *spvisNext;
			struct tagVIS *spvisParent;
			struct tagVIS *spvisChild;
			struct tagVIS *spvisOwner;
			RECT          rcWindow;
			RECT          rcClient;
			HRGN          hrgnUpdate;
			DWORD		  dwExStyle;
			DWORD		  dwStyle;
			DWORD		  dwState;
		}VIS, *PVIS;
//#define STATEOFFSET (sizeof(VIS) - sizeof(WW) + (WND_CNT_WOWDWORDS * sizeof(DWORD)))
//#define TestWF(hwnd, flag)   (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) & LOBYTE(flag))
//#define SetWF(hwnd, flag)    (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) |= LOBYTE(flag))
//#define ClrWF(hwnd, flag)    (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) &= ~LOBYTE(flag))
//#define MaskWF(flag)         ((WORD)( (HIBYTE(flag) & 1) ? LOBYTE(flag) << 8 : LOBYTE(flag)))

#define PVIS_INPUTOWNER (PVIS)1   
#define PVIS_FOCUS      (PVIS)NULL 
#define PVIS_ERROR      (PVIS)0x10  
#define PVIS_TOP        (PVIS)0
#define PVIS_BOTTOM     (PVIS)1
#define PVIS_GROUPTOTOP ((PVIS)-1)
#define PVIS_TOPMOST    ((PVIS)-1)

	/*	VOID LinkWindow(
			PVIS pwnd,
			PVIS pwndInsert,
			PVIS *ppwndFirst);
		VOID UnlinkWindow(
			PVIS pwndUnlink,
			PVIS *ppwndFirst);*/
	}
}



