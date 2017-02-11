#include "stdafx.h"
#include "Utility.h"

GraphicsObserver::GraphicsObserver()
{

}
GraphicsObserver::~GraphicsObserver()
{

}

BOOL WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList)
{
	HMODULE hModules[1024];
	DWORD dwCount = 0;
	if (EnumProcessModulesEx(hProcess, hModules, sizeof(hModules), &dwCount, LIST_MODULES_ALL))
	{
		for (UINT i = 0; i < (dwCount / sizeof(HMODULE)); i++)
		{
			TinyString szFileName(MAX_PATH);
			if (GetModuleFileNameEx(hProcess, hModules[i], szFileName.STR(), MAX_PATH))
			{
				TinyString str = szFileName.Substring(szFileName.LastIndexOf("\\") + 1, szFileName.GetSize());
				moduleList.Add(str);
			}
		}
		return TRUE;
	}
	return FALSE;
}
void WINAPI SaveBitmapToFile(const BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize)
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
BOOL WINAPI SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
{
	INT iBits;
	WORD wBitCount = 24;
	DWORD dwPaletteSize = 0;
	DWORD dwBmBitsSize = 0;
	DWORD dwDIBSize = 0;
	DWORD dwWritten = 0;
	BITMAP bitmap;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh = INVALID_HANDLE_VALUE;
	HGDIOBJ hPal = NULL, hOldPal = NULL;
	void* bits = NULL;
	BOOL bResult = FALSE;
	HDC hDisplayDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	HDC hDC = ::CreateCompatibleDC(hDisplayDC);
	if (hDC)
	{
		iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	}
	DeleteDC(hDC);
	DeleteDC(hDisplayDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 24;
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((bitmap.bmWidth * wBitCount + 31) / 32) * 4 * bitmap.bmHeight;
	bits = malloc(dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	if (bits == NULL)
		goto leave;
	lpbi = (LPBITMAPINFOHEADER)bits;
	*lpbi = bi;
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	GetDIBits(hDC, hBitmap, 0, (UINT)bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize,
		(LPBITMAPINFO)
		lpbi, DIB_RGB_COLORS);
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		goto leave;
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
		+ (DWORD)sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize;
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	bResult = TRUE;
leave:
	if (bits != NULL)
	{
		free(bits);
	}
	if (fh != INVALID_HANDLE_VALUE)
		CloseHandle(fh);

	return bResult;

}
BOOL WINAPI WindowSave(HWND hWND)
{
	HDC hDC = GetDC(hWND);
	HDC hMemDC = CreateCompatibleDC(hDC);
	TinyRectangle s;
	GetClientRect(hWND, &s);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, s.Size().cx, s.Size().cy);
	SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, s.Size().cx, s.Size().cy, hDC, s.Position().x, s.Position().y, SRCCOPY);
	SaveBitmapToFile(hBitmap, "D:\\1234.bmp");
	SAFE_DELETE_OBJECT(hBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(hWND, hDC);
	return TRUE;
}
BOOL WINAPI ScreenSave(const TinyRectangle& s)
{
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, s.Size().cx, s.Size().cy);
	SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, s.Size().cx, s.Size().cy, hDC, s.Position().x, s.Position().y, SRCCOPY);
	SaveBitmapToFile(hBitmap, "D:\\1234.bmp");
	SAFE_DELETE_OBJECT(hBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	return TRUE;
}
BOOL WINAPI InvertWindow(HWND hWND)
{
	if (!hWND)
		return FALSE;
	TinyRectangle rectangle;
	GetWindowRect(hWND, &rectangle);
	rectangle.OffsetRect(-rectangle.Position().x, -rectangle.Position().y);
	HDC hDC = GetWindowDC(hWND);
	if (hDC != NULL)
	{
		RECT s = { 0 };
		SetRect(&s, 0, 0, rectangle.right, 4);
		InvertRect(hDC, &s);
		SetRect(&s, 0, 4, 4, rectangle.bottom);
		InvertRect(hDC, &s);
		SetRect(&s, 4, rectangle.bottom - 4, rectangle.right, rectangle.bottom);
		InvertRect(hDC, &s);
		SetRect(&s, rectangle.right - 4, 4, rectangle.right, rectangle.bottom - 4);
		InvertRect(hDC, &s);
		ReleaseDC(hWND, hDC);
		return TRUE;
	}
	return FALSE;
}

