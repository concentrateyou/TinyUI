#pragma once
#include "Common/TinyCommon.h"
#include "Render/TinyGDI.h"
#include "Common/TinyCallback.h"
#include "Media/TinyMedia.h"
#include <mfxvideo.h>
#include <mfxvideo++.h>
#include <dxgi.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxva2api.h>
#include <vector>
#include <queue>
#include <list>
using namespace std;
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
#define MSDK_CHECK_STATUS(X, MSG)               {if ((X) < MFX_ERR_NONE) {MSDK_PRINT_RET_MSG(X, MSG); return X;}}
#define MSDK_SAFE_DELETE(P)                      { delete P; P = NULL; }
#define MSDK_SAFE_DELETE_ARRAY(P)                { delete[] P; P = NULL; }
#define MSDK_SAFE_RELEASE(P)                     { if (P) { P->Release(); P = NULL; } }

#define MSDK_ZERO_MEMORY(VAR)                    {memset(&VAR, 0, sizeof(VAR));}
#define MSDK_MAX(A, B)                           (((A) > (B)) ? (A) : (B))
#define MSDK_MIN(A, B)                           (((A) < (B)) ? (A) : (B))
#define MSDK_ALIGN16(value)                      (((value + 15) >> 4) << 4) // round up to a multiple of 16
#define MSDK_ALIGN32(value)                      (((value + 31) >> 5) << 5) // round up to a multiple of 32
#define MSDK_ALIGN(value, alignment)             (alignment) * ( (value) / (alignment) + (((value) % (alignment)) ? 1 : 0))
#define MSDK_ARRAY_LEN(value)                    (sizeof(value) / sizeof(value[0]))
#define MSDK_MEMCPY_BITSTREAM(bitstream, offset, src, count) memcpy((bitstream).Data + (offset), (src), (count))
#define MSDK_MEMCPY_BUF(bufptr, offset, maxsize, src, count) memcpy((bufptr)+ (offset), (src), (count))
#define MSDK_MEMCPY_VAR(dstVarName, src, count) memcpy(&(dstVarName), (src), (count))

#define MSDK_MEMCPY(dst, src, count) memcpy(dst, (src), (count))
#define D3DFMT_NV12 (D3DFORMAT)MAKEFOURCC('N','V','1','2')
#define D3DFMT_YV12 (D3DFORMAT)MAKEFOURCC('Y','V','1','2')
#define D3DFMT_NV16 (D3DFORMAT)MAKEFOURCC('N','V','1','6')
#define D3DFMT_P010 (D3DFORMAT)MAKEFOURCC('P','0','1','0')
#define D3DFMT_P210 (D3DFORMAT)MAKEFOURCC('P','2','1','0')
#define D3DFMT_Y210 (D3DFORMAT)MAKEFOURCC('Y','2','1','0')
#define D3DFMT_IMC3 (D3DFORMAT)MAKEFOURCC('I','M','C','3')
#define D3DFMT_AYUV (D3DFORMAT)MAKEFOURCC('A','Y','U','V')

#define MFX_FOURCC_IMC3 (MFX_MAKEFOURCC('I','M','C','3')) // This line should be moved into mfxstructures.h in new API version

	D3DFORMAT ConvertMfxFourccToD3dFormat(mfxU32 fourcc);

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

