#include "stdafx.h"
#include "Utility.h"

INT ToINT32(BYTE val[4])
{
	return (INT)(((val[0] & 0xFF) << 24) |
		((val[1] & 0xFF) << 16) |
		((val[2] & 0xFF) << 8) |
		((val[3] & 0xFF)));
}
INT ToINT24(BYTE val[3])
{
	return (INT)(((val[0] & 0xFF) << 16) |
		((val[1] & 0xFF) << 8) |
		((val[2] & 0xFF)));
}
INT ToINT16(BYTE val[2])
{
	return	(INT)(val[0] << 8) | (val[1]);
}
INT ToINT8(BYTE val[1])
{
	return	(INT)(val[0] << 8);
}
//void WINAPI SaveBitmap(const BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize)
//{
//	BITMAPFILEHEADER  bmfHeader = { 0 };
//	DWORD dwSizeofDIB = dwSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
//	bmfHeader.bfSize = dwSizeofDIB;
//	bmfHeader.bfType = 0x4D42;
//	HANDLE hFile = CreateFile("D:\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	DWORD dwBytesWritten = 0;
//	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
//	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
//	WriteFile(hFile, (LPSTR)pBits, dwSize, &dwBytesWritten, NULL);
//	CloseHandle(hFile);
//}