#include "../stdafx.h"
#include "TinyImage.h"
#include "../Network/TinyHTTPClient.h"
#include "../Common/TinyLogging.h"
using namespace TinyUI::Network;

namespace TinyUI
{

#ifdef __cplusplus
	extern "C" {
#endif
		extern unsigned char *stbi_load_ex(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
		extern unsigned char *stbi_load_from_memory_ex(unsigned char const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
		extern unsigned char *stbi_load_from_file_ex(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
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
		m_count(1),
		m_hBitmap(NULL)
	{

	}
	TinyImage::~TinyImage()
	{
		Close();
	}
	BOOL TinyImage::IsEmpty() const
	{
		return m_hBitmap == NULL || m_count == 0;
	}

	void TinyImage::Close()
	{
		for (INT i = 0; i < m_images.GetSize(); i++)
		{
			SAFE_DELETE_OBJECT(m_images[i].hBitmap);
		}
		m_images.RemoveAll();
		m_hBitmap = NULL;
		m_count = 1;
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
	BOOL TinyImage::Open(BYTE* bits, DWORD size)
	{
		if (!bits || size <= 0)
			return S_FALSE;
		Close();
		INT comp = 0;
		BYTE* pData = stbi_load_from_memory_ex(bits, size, &m_cx, &m_cy, &comp, 4, &m_count);
		if (!pData)
		{
			goto _ERROR;
		}
		if (m_count == 1)
		{
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_cx;
			bmi.bmiHeader.biHeight = -m_cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
			IMAGE_INFO s = { 0 };
			s.hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&s.Bits, NULL, 0);
			if (s.hBitmap)
			{
				for (INT i = 0; i < m_cx * m_cy; i++)
				{
					s.Bits[i * 4 + 3] = pData[i * 4 + 3];
					if (s.Bits[i * 4 + 3] < 255)
					{
						s.Bits[i * 4] = (BYTE)(DWORD(pData[i * 4 + 2])*pData[i * 4 + 3] / 255);//B
						s.Bits[i * 4 + 1] = (BYTE)(DWORD(pData[i * 4 + 1])*pData[i * 4 + 3] / 255);//G
						s.Bits[i * 4 + 2] = (BYTE)(DWORD(pData[i * 4])*pData[i * 4 + 3] / 255);//R
					}
					else
					{
						s.Bits[i * 4] = pData[i * 4 + 2];
						s.Bits[i * 4 + 1] = pData[i * 4 + 1];
						s.Bits[i * 4 + 2] = pData[i * 4];
					}
				}
				m_images.Add(s);
			}
		}
		else
		{
			BYTE* seek = pData + m_count * 4 * m_cx * m_cy;
			for (INT i = 0; i < m_count; i++)
			{
				BITMAPINFO bmi;
				memset(&bmi, 0, sizeof(BITMAPINFO));
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = m_cx;
				bmi.bmiHeader.biHeight = -m_cy;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
				IMAGE_INFO s = { 0 };
				s.hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&s.Bits, NULL, 0);
				if (s.hBitmap)
				{
					BYTE* ps = pData + i * m_cx * m_cy;
					for (INT i = 0; i < m_cx * m_cy; i++)
					{
						s.Bits[i * 4 + 3] = ps[i * 4 + 3];
						if (s.Bits[i * 4 + 3] < 255)
						{
							s.Bits[i * 4] = (BYTE)(DWORD(ps[i * 4 + 2])*ps[i * 4 + 3] / 255);//B
							s.Bits[i * 4 + 1] = (BYTE)(DWORD(ps[i * 4 + 1])*ps[i * 4 + 3] / 255);//G
							s.Bits[i * 4 + 2] = (BYTE)(DWORD(ps[i * 4])*ps[i * 4 + 3] / 255);//R
						}
						else
						{
							s.Bits[i * 4] = ps[i * 4 + 2];
							s.Bits[i * 4 + 1] = ps[i * 4 + 1];
							s.Bits[i * 4 + 2] = ps[i * 4];
						}
					}
					s.Delay = (*(UINT*)seek) * 10;
					m_images.Add(s);
					seek += sizeof(UINT);
				}
			}
		}
		stbi_image_free(pData);
		m_hBitmap = m_images.GetSize() == 0 ? NULL : m_images[0].hBitmap;
		return m_images.GetSize() == m_count ? TRUE : FALSE;
	_ERROR:
		stbi_image_free(pData);
		return FALSE;
	}

	BOOL TinyImage::OpenFile(LPCSTR pzFile)
	{
		if (!pzFile)
			return FALSE;
		Close();
		FILE* pFile = NULL;
		if (fopen_s(&pFile, pzFile, "rb") || !pFile)
			return S_FALSE;
		//解码出来的数据是RGBA
		INT comp = 0;
		BYTE* pData = NULL;
		pData = stbi_load_from_file_ex(pFile, &m_cx, &m_cy, &comp, 4, &m_count);
		fclose(pFile);
		if (pData == NULL)
		{
			goto _ERROR;
		}
		if (m_count == 1)
		{
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_cx;
			bmi.bmiHeader.biHeight = -m_cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
			IMAGE_INFO s = { 0 };
			s.hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&s.Bits, NULL, 0);
			if (s.hBitmap != NULL)
			{
				for (INT i = 0; i < m_cx * m_cy; i++)
				{
					s.Bits[i * 4 + 3] = pData[i * 4 + 3];
					if (s.Bits[i * 4 + 3] < 255)
					{
						s.Bits[i * 4] = (BYTE)(DWORD(pData[i * 4 + 2])*pData[i * 4 + 3] / 255);//B
						s.Bits[i * 4 + 1] = (BYTE)(DWORD(pData[i * 4 + 1])*pData[i * 4 + 3] / 255);//G
						s.Bits[i * 4 + 2] = (BYTE)(DWORD(pData[i * 4])*pData[i * 4 + 3] / 255);//R
					}
					else
					{
						s.Bits[i * 4] = pData[i * 4 + 2];
						s.Bits[i * 4 + 1] = pData[i * 4 + 1];
						s.Bits[i * 4 + 2] = pData[i * 4];
					}
				}
				m_images.Add(s);
			}
		}
		else
		{
			BYTE* bits = pData + m_count * 4 * m_cx * m_cy;
			for (INT i = 0; i < m_count; i++)
			{
				BYTE* ps = pData + i * 4 * m_cx * m_cy;
				BITMAPINFO bmi;
				memset(&bmi, 0, sizeof(BITMAPINFO));
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = m_cx;
				bmi.bmiHeader.biHeight = -m_cy;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = m_cx * m_cy * 4;
				IMAGE_INFO s = { 0 };
				s.hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&s.Bits, NULL, 0);
				if (s.hBitmap != NULL)
				{
					for (INT i = 0; i < m_cx * m_cy; i++)
					{
						s.Bits[i * 4 + 3] = ps[i * 4 + 3];
						if (s.Bits[i * 4 + 3] < 255)
						{
							s.Bits[i * 4] = (BYTE)(DWORD(ps[i * 4 + 2])*ps[i * 4 + 3] / 255);//B
							s.Bits[i * 4 + 1] = (BYTE)(DWORD(ps[i * 4 + 1])*ps[i * 4 + 3] / 255);//G
							s.Bits[i * 4 + 2] = (BYTE)(DWORD(ps[i * 4])*ps[i * 4 + 3] / 255);//R
						}
						else
						{
							s.Bits[i * 4] = ps[i * 4 + 2];
							s.Bits[i * 4 + 1] = ps[i * 4 + 1];
							s.Bits[i * 4 + 2] = ps[i * 4];
						}
					}
					s.Delay = (*(UINT*)bits) * 10;
					m_images.Add(s);
					bits += sizeof(UINT);
				}
			}
		}
		stbi_image_free(pData);
		m_hBitmap = m_images.GetSize() == 0 ? NULL : m_images[0].hBitmap;
		return m_images.GetSize() == m_count ? TRUE : FALSE;
	_ERROR:
		stbi_image_free(pData);
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
	INT TinyImage::GetCount() const
	{
		return m_count;
	}
	HBITMAP	TinyImage::GetHBITMAP(INT index)
	{
		if (index < 0 || index >= (INT)m_count)
		{
			return NULL;
		}
		return m_images[index].hBitmap;
	}
	BYTE* TinyImage::GetBits(INT index)
	{
		if (index < 0 || index >= (INT)m_count)
		{
			return NULL;
		}
		return m_images[index].Bits;
	}
	INT	TinyImage::GetDelay(INT index)
	{
		if (index < 0 || index >= (INT)m_count)
		{
			return -1;
		}
		return m_images[index].Delay;
	}
	TinyImage::operator HBITMAP() const
	{
		return m_hBitmap;
	}
}
