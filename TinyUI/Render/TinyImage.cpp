#include "../stdafx.h"
#include "TinyImage.h"
#include "../Network/TinyHTTPClient.h"
#include "../Common/TinyLogging.h"
#include <fstream>
using namespace std;
using namespace TinyUI::Network;

namespace TinyUI
{

#ifdef __cplusplus
	extern "C" {
#endif
		extern unsigned char *stbi_load_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
		extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
		extern unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
		extern void	stbi_image_free(void *retval_from_stbi_load);

#ifdef __cplusplus
	}
#endif

	BOOL WINAPI SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
	{
		INT iBits;
		WORD wBitCount = 32;
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
			wBitCount = 32;
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

	TinyImage::TinyImage()
		:m_cx(0),
		m_cy(0),
		m_hBitmap(NULL),
		m_bits(NULL)
	{

	}
	TinyImage::~TinyImage()
	{
		Close();
	}
	BOOL TinyImage::IsEmpty() const
	{
		return m_hBitmap == NULL;
	}

	void TinyImage::Close()
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		m_cx = m_cy = 0;
	}

	BOOL TinyImage::Open(LPCSTR pzFile)
	{
		if (PathIsURL(pzFile))
		{
			TinyHTTPClient client;
			client.SetTimeout(5000);
			client.GetRequest().SetVerbs(TinyHTTPClient::GET);
			client.GetRequest().Add("Connection", "keep-alive");
			if (client.Open(pzFile))
			{
				CHAR* ps = NULL;
				INT iRes = client.GetResponse().ReadAsBinary(ps);
				if (iRes > 0 && ps != NULL)
				{
					LOG(INFO) << "Open Image: " << pzFile << " OK";
					return Open((BYTE*)ps, iRes);
				}
				else
				{
					LOG(ERROR) << "Open Image: " << iRes << " FAIL";
				}
			}
		}
		else
		{
			return OpenFile(pzFile);
		}
		return FALSE;
	}
	BOOL TinyImage::Open(BYTE* m_bits, DWORD size)
	{
		if (!m_bits || size <= 0)
			return S_FALSE;
		Close();
		INT comp = 0;
		BYTE* data = NULL;
		data = stbi_load_from_memory(m_bits, size, &m_cx, &m_cy, &comp, 4);
		if (!data)
		{
			LOG(ERROR) << "stbi_load_from_memory : " << size << " FAIL";
			goto _ERROR;
		}
		else
		{
			LOG(INFO) << "stbi_load_from_memory : " << size << " OK";
		}
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_cx;
		bmi.bmiHeader.biHeight = -m_cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
		m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&m_bits, NULL, 0);
		if (m_hBitmap != NULL)
		{
			for (INT i = 0; i < m_cx * m_cy; i++)
			{
				m_bits[i * 4 + 3] = data[i * 4 + 3];
				if (m_bits[i * 4 + 3] < 255)
				{
					m_bits[i * 4] = (BYTE)(DWORD(data[i * 4 + 2])*data[i * 4 + 3] / 255);//B
					m_bits[i * 4 + 1] = (BYTE)(DWORD(data[i * 4 + 1])*data[i * 4 + 3] / 255);//G
					m_bits[i * 4 + 2] = (BYTE)(DWORD(data[i * 4])*data[i * 4 + 3] / 255);//R
				}
				else
				{
					m_bits[i * 4] = data[i * 4 + 2];
					m_bits[i * 4 + 1] = data[i * 4 + 1];
					m_bits[i * 4 + 2] = data[i * 4];
				}
			}
		}
		if (data != NULL)
		{
			stbi_image_free(data);
			data = NULL;
		}
		return m_hBitmap != NULL;
	_ERROR:
		if (data != NULL)
		{
			stbi_image_free(data);
			data = NULL;
		}
		Close();
		return FALSE;
	}

	BOOL TinyImage::OpenFile(LPCSTR pzFile)
	{
		if (!pzFile)
			return FALSE;
		Close();
		FILE* hFile = NULL;
		if (fopen_s(&hFile, pzFile, "rb") || !hFile)
			return S_FALSE;
		//解码出来的数据是RGBA
		INT comp = 0;
		BYTE* data = NULL;
		data = stbi_load_from_file(hFile, &m_cx, &m_cy, &comp, 4);
		fclose(hFile);
		if (!data)
		{
			LOG(ERROR) << "stbi_load_from_file FAIL";
			goto _ERROR;
		}
		else
		{
			LOG(INFO) << "stbi_load_from_file OK";
		}
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_cx;
		bmi.bmiHeader.biHeight = -m_cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
		BYTE* bits = NULL;
		m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
		if (m_hBitmap != NULL)
		{
			for (INT i = 0; i < m_cx * m_cy; i++)
			{
				bits[i * 4 + 3] = data[i * 4 + 3];
				if (bits[i * 4 + 3] < 255)
				{
					bits[i * 4] = (BYTE)(DWORD(data[i * 4 + 2])*data[i * 4 + 3] / 255);//B
					bits[i * 4 + 1] = (BYTE)(DWORD(data[i * 4 + 1])*data[i * 4 + 3] / 255);//G
					bits[i * 4 + 2] = (BYTE)(DWORD(data[i * 4])*data[i * 4 + 3] / 255);//R
				}
				else
				{
					bits[i * 4] = data[i * 4 + 2];
					bits[i * 4 + 1] = data[i * 4 + 1];
					bits[i * 4 + 2] = data[i * 4];
				}
			}
		}
		if (data != NULL)
		{
			stbi_image_free(data);
			data = NULL;
		}
		return m_hBitmap != NULL;
	_ERROR:
		if (data != NULL)
		{
			stbi_image_free(data);
			data = NULL;
		}
		SAFE_DELETE_OBJECT(m_hBitmap);
		return FALSE;
	}

	TinySize TinyImage::GetSize()
	{
		return TinySize(m_cx, m_cy);
	}
	TinyRectangle TinyImage::GetRectangle()
	{
		return TinyRectangle(0, 0, m_cx, m_cy);
	}
	BOOL TinyImage::Save(LPCSTR pz)
	{
		return SaveBitmapToFile(m_hBitmap, pz);
	}
	HBITMAP	TinyImage::GetHBITMAP()
	{
		return m_hBitmap;
	}
	BYTE* TinyImage::GetBits()
	{
		return m_bits;
	}
	TinyImage::operator HBITMAP() const
	{
		return m_hBitmap;
	}
}
