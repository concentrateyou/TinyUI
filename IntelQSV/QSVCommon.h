#pragma once
#include "Common/TinyCommon.h"
#include "Render/TinyGDI.h"
#include "Common/TinyCallback.h"
#include "Media/TinyMedia.h"
#include "common/common_utils.h"
#include <vector>
#include <dxgi.h>
using namespace TinyUI;

namespace QSV
{
	typedef struct tagQSVParam
	{
		WORD wCX;
		WORD wCY;
		WORD wFPS;
		WORD wKbps;
		WORD wMaxKbps;
		WORD wAccuracy;
		WORD wConvergence;
		WORD wQPI;
		WORD wQPB;
		WORD wQPP;
		WORD wRC;
		WORD wKeyPerSec;//每秒关键帧数
		WORD wbFrames;//b帧数
		WORD wAsyncDepth;
		WORD wTargetUsage;
	}QSVParam;

#define MSDK_PRINT_RET_MSG(ERR) \
{\
    TCHAR msg[1024];    \
    _stprintf_s(msg, MSDK_ARRAY_LEN(msg), _T("\nReturn on error: error code %d,\t%s\t%d\n\n"), ERR, _T(__FILE__), __LINE__); \
    OutputDebugString(msg); \
    _tperror(msg);\
}
#define MSDK_SUCCEEDED(P)                        (MFX_ERR_NONE == (P))
#define MSDK_FAILED(P)                           (MFX_ERR_NONE != (P))
#define MSDK_CHECK_ERROR(P, X, ERR)              { if ((X) == (P)) { MSDK_PRINT_RET_MSG(ERR); return ERR; } }
#define MSDK_CHECK_NOT_EQUAL(P, X, ERR)          { if ((X) != (P)) { MSDK_PRINT_RET_MSG(ERR); return ERR; } }
#define MSDK_CHECK_RESULT_P_RET(P, X)            { if ((X) != (P)) { return P; } }
#define MSDK_CHECK_POINTER(P, ERR)               { if (!(P))       { return ERR;}}
#define MSDK_CHECK_POINTER_NO_RET(P)             { if (!(P))       { return; } }
#define MSDK_IGNORE_MFX_STS(P, X)                { if ((X) == (P)) { P = MFX_ERR_NONE; } }

#define MSDK_SAFE_DELETE(P)                      { delete P; P = NULL; }
#define MSDK_SAFE_DELETE_ARRAY(P)                { delete[] P; P = NULL; }
#define MSDK_SAFE_RELEASE(P)                     { if (P) { P->Release(); P = NULL; } }

#define MSDK_ZERO_MEMORY(P, S)                   { memset(P, 0, S);}
#define MSDK_ZERO_VAR(VAR)                       { memset(&VAR, 0, sizeof(VAR)); }
#define MSDK_ALIGN16(SZ)                         ((SZ + 15) & (~15)) // round up to a multiple of 16
#define MSDK_ALIGN32(SZ)                         ((SZ + 31) & (~31)) // round up to a multiple of 32
#define MSDK_ALIGN64(SZ)                         ((SZ + 63) & (~63)) // round up to a multiple of 64
#define MSDK_ALIGN128(SZ)                        ((SZ + 127) & (~127)) // round up to a multiple of 128

	const struct
	{
		mfxIMPL impl;      
		mfxU32  adapterID; 
	} implTypes[] =
	{
		{ MFX_IMPL_HARDWARE, 0 },
		{ MFX_IMPL_HARDWARE2, 1 },
		{ MFX_IMPL_HARDWARE3, 2 },
		{ MFX_IMPL_HARDWARE4, 3 }
	};
	mfxU32 GetIntelAdapter(mfxSession session);
}

