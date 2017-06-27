#include "../stdafx.h"
#include "TinyImage.h"

namespace TinyUI
{
	extern "C"
	{
		extern unsigned char *stbi_load_ex(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
		extern unsigned char *stbi_load_from_memory_ex(unsigned char const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
		extern unsigned char *stbi_load_from_file_ex(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels, int* count);
		extern void	stbi_image_free(void *retval_from_stbi_load);
	};

	TinyImage::TinyImage()
		:m_cx(0),
		m_cy(0),
		m_count(1),
		m_hBitmap(NULL)
	{

	}
	TinyImage::~TinyImage()
	{
		Release();
	}
	BOOL TinyImage::IsEmpty() const
	{
		return m_hBitmap == NULL || m_count == 0;
	}

	void TinyImage::Release()
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

	BOOL TinyImage::Load(LPCSTR pz)
	{
		Release();
		FILE* pFile = NULL;
		if (fopen_s(&pFile, pz, "rb") || !pFile)
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
	BOOL TinyImage::Load(BYTE* bits, DWORD size)
	{
		for (INT i = 0; i < m_images.GetSize(); i++)
		{
			SAFE_DELETE_OBJECT(m_images[i].hBitmap);
		}
		if (!bits)
			return S_FALSE;
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
		return TRUE;
	}
	size_t TinyImage::GetCount()
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
