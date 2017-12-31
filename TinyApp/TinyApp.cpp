// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "ChatDlg.h"
#include "MFFrame.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualContextMenu.h"
#include "Render/TinyDDraw.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Media/TinyWave.h"
#include "Common/TinyHook.h"
#include "Media/TinyMFMP3Decode.h"
#include "MPG123Decode.h"
#include "FLVParser.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "Common/TinySignal.h"
#include "SkinWindow.h"
#include "Media/TinyWave.h"
#include "FLVParser.h"
#include "TSReader.h"
#include "Windowless/TinyVisualLayeredWindow.h"
#include "Media/TinySoundCapture.h"
#include "QSVEncoder.h"
#include <fstream>

using namespace TinyUI;
using namespace TinyUI::Network;
using namespace TinyUI::Media;
using namespace Decode;

BOOL LoadSeDebugPrivilege()
{
	DWORD   err;
	HANDLE  hToken;
	LUID    Val;
	TOKEN_PRIVILEGES tp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		err = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}

BOOL WINAPI SaveBitmapToFile1(HBITMAP hBitmap, LPCTSTR lpFileName)
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

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);

	HRESULT hRes = OleInitialize(NULL);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	LoadSeDebugPrivilege();

	/*TinyImage image1;
	image1.Open("D:\\1.png");*/

	TinyImage image;
	if (image.Open("http://n.sinaimg.cn/news/transform/w1000h500/20171229/-r10-fypyuve2758447.jpg")) 
	{
		image.Save("D:\\123.bmp");
	}
	/*TinyVisualResource::GetInstance().Load("skin\\resource.xml");
	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	TinyVisualLayeredWindow uiImpl;
	string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\contextmenu.xml");
	uiImpl.Create(NULL, szFile.c_str());
	TinyVisualContextMenu* contextmenu = (TinyVisualContextMenu*)uiImpl.GetDocument().GetVisualByName("menu1");
	contextmenu->Add("Test1");
	contextmenu->Add("Test2");
	contextmenu->Add("Test3");
	uiImpl.Update();*/
	/*INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();*/

	OleUninitialize();
	MFShutdown();
	WSACleanup();

	return 0;
};