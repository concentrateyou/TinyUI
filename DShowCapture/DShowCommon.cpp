#include "stdafx.h"
#include "DShowCommon.h"

GUID MediaSubTypeI420 =
{
	0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }
};
GUID MediaSubTypeHDYC =
{
	0x43594448, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
};
const REFERENCE_TIME SecondsToReferenceTime = 10000000;

HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
	ASSERT(pmtSource != pmtTarget);
	*pmtTarget = *pmtSource;
	if (pmtSource->cbFormat != NULL)
	{
		ASSERT(pmtSource->pbFormat != NULL);
		pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
		if (pmtTarget->pbFormat == NULL)
		{
			pmtTarget->cbFormat = 0;
			return E_OUTOFMEMORY;
		}
		else
		{
			CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat, pmtTarget->cbFormat);
		}
	}
	if (pmtTarget->pUnk != NULL)
	{
		pmtTarget->pUnk->AddRef();
	}
	return NOERROR;
}
void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}
void WINAPI DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	if (pmt)
	{
		FreeMediaType(*pmt);
		CoTaskMemFree((PVOID)pmt);
	}
}

void WINAPI SaveBitmap(const BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize)
{
	BITMAPFILEHEADER  bmfHeader = { 0 };
	DWORD dwSizeofDIB = dwSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	HANDLE hFile = CreateFile("D:\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)pBits, dwSize, &dwBytesWritten, NULL);
	CloseHandle(hFile);
}