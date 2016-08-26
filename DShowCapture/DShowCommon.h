#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyString.h"
#include "Render/TinyGDI.h"
#include "Common/TinyCallback.h"
#include <dshow.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;
using namespace TinyUI;

extern const REFERENCE_TIME SecondsToReferenceTime;
extern GUID MediaSubTypeI420;
extern GUID MediaSubTypeHDYC;

#define FILTER_NAME  L"IQiyi Capture Filter"
#define PIN_NAME     L"IQiyi Capture"

HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource);
void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt);
void WINAPI DeleteMediaType(AM_MEDIA_TYPE *pmt);
void WINAPI SaveBitmap(BITMAPINFOHEADER bi, const BYTE* pBits, DWORD dwSize);
