#include "stdafx.h"
#include "Utility.h"

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
BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
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

void CHARFORMAT2LOGFONT(const CHARFORMAT& cf, LOGFONT& lf, COLORREF& color)
{
	::ZeroMemory(&lf, sizeof(CHARFORMAT));
	lf.lfCharSet = cf.bCharSet;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfWeight = (cf.dwEffects & CFE_BOLD) == CFE_BOLD ? FW_BOLD : 0;
	TinyDC dc(CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL));
	lf.lfHeight = -MulDiv(cf.yHeight / 20, dc.GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfUnderline = ((cf.dwEffects & CFE_UNDERLINE) == CFE_UNDERLINE);
	lf.lfStrikeOut = ((cf.dwEffects & CFE_STRIKEOUT) == CFE_STRIKEOUT);
	lf.lfItalic = ((cf.dwEffects & CFE_ITALIC) == CFE_ITALIC);
	lf.lfWidth = 0;
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, cf.szFaceName);
	color = cf.crTextColor;
}
void LOGFONT2CHARFORMAT(const LOGFONT& lf, CHARFORMAT& cf, const COLORREF& color)
{
	::ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_OFFSET | CFM_PROTECTED | CFM_SIZE | CFM_STRIKEOUT | CFM_UNDERLINE;
	cf.dwEffects = CFE_AUTOCOLOR | (lf.lfWeight >= FW_BOLD ? CFE_BOLD : 0) | (lf.lfItalic ? CFE_ITALIC : 0) | (lf.lfUnderline ? CFE_UNDERLINE : 0) | (lf.lfStrikeOut ? CFE_STRIKEOUT : 0);
	TinyDC dc(CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL));
	cf.yHeight = 20 * LONG(0.5 + fabs(DOUBLE(72 * lf.lfHeight) / dc.GetDeviceCaps(LOGPIXELSY)));
	cf.yOffset = 0;
	cf.crTextColor = color;
	cf.bCharSet = lf.lfCharSet;
	cf.bPitchAndFamily = lf.lfPitchAndFamily;
	_tcscpy_s(cf.szFaceName, LF_FACESIZE, lf.lfFaceName);
}

Gdiplus::RectF WINAPI MeasureString(HDC hDC, const wstring& str, const CHARFORMAT& cf)
{
	Gdiplus::Graphics graphics(hDC);
	LOGFONT lf;
	COLORREF color;
	CHARFORMAT2LOGFONT(cf, lf, color);
	Gdiplus::Font font(hDC, &lf);
	Gdiplus::RectF boundingBox;
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
	format.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
	format.SetTrimming(Gdiplus::StringTrimmingWord);
	graphics.MeasureString(str.c_str(), str.size(), &font, PointF(0.0, 0.0), &format, &boundingBox);
	return boundingBox;
}


