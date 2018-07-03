#include "../stdafx.h"
#include "TinyImage.h"
#include "../Network/TinyHTTPClient.h"
#include "../Common/TinyLogging.h"
#include <fstream>
using namespace std;
using namespace TinyFramework::Network;

namespace TinyFramework
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
		:m_hBitmap(NULL),
		m_bits(NULL),
		m_cx(0),
		m_cy(0)
	{

	}
	TinyImage::TinyImage(const TinyImage& o)
	{
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = o.m_cx;
		bmi.bmiHeader.biHeight = -o.m_cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = o.m_cx * o.m_cy * 4;
		BYTE* bits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
		if (hBitmap != NULL)
		{
			this->Close();
			m_bits = bits;
			m_hBitmap = hBitmap;
			m_cx = o.m_cx;
			m_cy = o.m_cy;
			memcpy(m_bits, bits, bmi.bmiHeader.biSizeImage);
		}
	}
	TinyImage::TinyImage(TinyImage&& o)
		:m_hBitmap(o.m_hBitmap),
		m_bits(o.m_bits),
		m_cx(o.m_cx),
		m_cy(o.m_cy)
	{
		o.m_bits = NULL;
		o.m_hBitmap = NULL;
		o.m_cx = 0;
		o.m_cy = 0;
	}
	TinyImage::~TinyImage()
	{
		Close();
	}
	void TinyImage::operator=(const TinyImage& o)
	{
		if (&o != this)
		{
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = o.m_cx;
			bmi.bmiHeader.biHeight = -o.m_cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = o.m_cx * o.m_cy * 4;
			BYTE* bits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
			if (hBitmap != NULL)
			{
				this->Close();
				m_bits = bits;
				m_hBitmap = hBitmap;
				m_cx = o.m_cx;
				m_cy = o.m_cy;
				memcpy(m_bits, bits, bmi.bmiHeader.biSizeImage);
			}
		}
	}
	void TinyImage::operator=(TinyImage&& o)
	{
		m_bits = o.m_bits;
		m_cx = o.m_cx;
		m_cy = o.m_cy;
		m_hBitmap = o.m_hBitmap;
		o.m_bits = NULL;
		o.m_hBitmap = NULL;
		o.m_cx = 0;
		o.m_cy = 0;
	}
	BOOL TinyImage::IsEmpty() const
	{
		return m_hBitmap == NULL;
	}

	void TinyImage::Close()
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		m_bits = NULL;
		m_cx = 0;
		m_cy = 0;
	}
	BOOL TinyImage::Create(HDC hDC, LPCSTR pzText, UINT format, COLORREF color)
	{
		if (!pzText)
			return FALSE;
		TinyRectangle s;
		DrawText(hDC, pzText, strlen(pzText), &s, DT_CALCRECT | format);
		m_cx = TO_CX(s);
		m_cy = TO_CY(s);
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_cx;
		bmi.bmiHeader.biHeight = m_cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&m_bits, NULL, 0);
		if (!m_hBitmap)
			return FALSE;
		TinyMemDC memDC(hDC, m_hBitmap);
		memDC.SelectObject(GetCurrentObject(hDC, OBJ_FONT));
		memDC.SetBkMode(TRANSPARENT);
		memDC.SetTextColor(GetTextColor(hDC));
		memDC.FillSolidRect(s.left, s.top, s.right, s.bottom, color);
		memDC.DrawText(pzText, strlen(pzText), &s, format);
		for (INT i = 0; i < m_cx * m_cy; i++)
		{
			COLORREF value = RGB(m_bits[i * 4 + 2], m_bits[i * 4 + 1], m_bits[i * 4]);
			if (value != color)
			{
				m_bits[i * 4 + 3] = 255;
			}
		}
		return TRUE;
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
	BOOL TinyImage::Open(BYTE* m_bits, LONG size)
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
	//////////////////////////////////////////////////////////////////////////
	static BOOL g_WIC2 = FALSE;
	IWICImagingFactory* GetImagingFactory()
	{
		static INIT_ONCE s_initOnce = INIT_ONCE_STATIC_INIT;
		IWICImagingFactory* factory = nullptr;
		InitOnceExecuteOnce(&s_initOnce,
			[](PINIT_ONCE, PVOID, PVOID *factory) -> BOOL
		{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
			HRESULT hRes = CoCreateInstance(
				CLSID_WICImagingFactory2,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory2),
				factory
			);

			if (SUCCEEDED(hRes))
			{
				g_WIC2 = TRUE;
				return TRUE;
			}
			else
			{
				hRes = CoCreateInstance(
					CLSID_WICImagingFactory1,
					nullptr,
					CLSCTX_INPROC_SERVER,
					__uuidof(IWICImagingFactory),
					factory
				);
				return SUCCEEDED(hRes) ? TRUE : FALSE;
			}
#else
			return SUCCEEDED(CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory),
				factory)) ? TRUE : FALSE;
#endif
		}, NULL, reinterpret_cast<LPVOID*>(&factory));
		return factory;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyWICImage::TinyWICImage()
		:m_hBitmap(NULL),
		m_cx(0),
		m_cy(0),
		m_bits(NULL)
	{

	}
	TinyWICImage::~TinyWICImage()
	{
		Close();
	}
	TinyWICImage::operator HBITMAP() const
	{
		return m_hBitmap;
	}
	BOOL TinyWICImage::IsEmpty() const
	{
		return m_hBitmap == NULL;
	}
	BOOL TinyWICImage::Open(LPCSTR pzFile)
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
					std::ofstream temp("D:\\temp.png", std::ios::binary);
					temp.write(ps, iRes);
					temp.close();
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
	BOOL TinyWICImage::OpenFile(LPCSTR pzFile)
	{
		auto pWIC = GetImagingFactory();
		if (!pWIC)
			return FALSE;
		wstring sz = StringToWString(pzFile);
		TinyComPtr<IWICBitmapDecoder> decoder;
		HRESULT hRes = pWIC->CreateDecoderFromFilename(sz.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapSource> source;
		hRes = WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, frame, &source);
		if (hRes != S_OK)
			return FALSE;
		hRes = source->GetSize((UINT*)&m_cx, (UINT*)&m_cy);
		if (hRes != S_OK)
			return FALSE;
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
			UINT stride = m_cx * 4;
			stride = ((stride + sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);
			hRes = source->CopyPixels(NULL, stride, stride * m_cy, static_cast<BYTE *>(m_bits));
			if (hRes != S_OK)
				goto _ERROR;
		}
		return TRUE;
	_ERROR:
		SAFE_DELETE_OBJECT(m_hBitmap);
		return FALSE;
	}
	BOOL TinyWICImage::Open(BYTE* bits, LONG size)
	{
		auto pWIC = GetImagingFactory();
		if (!pWIC)
			return FALSE;
		TinyComPtr<IWICStream>	stream;
		HRESULT hRes = pWIC->CreateStream(&stream);
		if (hRes != S_OK)
			return FALSE;
		hRes = stream->InitializeFromMemory(reinterpret_cast<BYTE*>(bits), size);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IWICBitmapDecoder> decoder;
		hRes = pWIC->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapSource> source;
		hRes = WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, frame, &source);
		if (hRes != S_OK)
			return FALSE;
		hRes = source->GetSize((UINT*)&m_cx, (UINT*)&m_cy);
		if (hRes != S_OK)
			return FALSE;
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
			UINT stride = m_cx * 4;
			stride = ((stride + sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);
			hRes = source->CopyPixels(NULL, stride, stride * m_cy, static_cast<BYTE *>(m_bits));
			if (hRes != S_OK)
				goto _ERROR;
		}
		return TRUE;
	_ERROR:
		SAFE_DELETE_OBJECT(m_hBitmap);
		return FALSE;
	}
	BOOL TinyWICImage::Save(LPCSTR pzFile)
	{
		PTSTR s = PathFindExtension(pzFile);
		GUID wicFormat = GUID_ContainerFormatPng;
		if (strcasecmp(s, ".bmp") == 0)
		{
			wicFormat = GUID_ContainerFormatBmp;
		}
		if (strcasecmp(s, ".jpg") == 0)
		{
			wicFormat = GUID_ContainerFormatJpeg;
		}
		if (strcasecmp(s, ".png") == 0)
		{
			wicFormat = GUID_ContainerFormatPng;
		}
		auto pWIC = GetImagingFactory();
		if (!pWIC)
			return FALSE;
		TinyComPtr<IWICStream> stream;
		HRESULT hRes = pWIC->CreateStream(&stream);
		if (hRes != S_OK)
			return FALSE;
		wstring ws = StringToWString(pzFile);
		hRes = stream->InitializeFromFilename(ws.c_str(), GENERIC_WRITE);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IWICBitmapEncoder> encoder;
		hRes = pWIC->CreateEncoder(wicFormat, NULL, &encoder);
		if (hRes != S_OK)
			return FALSE;
		hRes = encoder->Initialize(stream, WICBitmapEncoderNoCache);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IWICBitmapFrameEncode> frame;
		hRes = encoder->CreateNewFrame(&frame, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = frame->Initialize(NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = frame->SetSize(m_cx, m_cy);
		if (hRes != S_OK)
			return FALSE;
		WICPixelFormatGUID wicPixelFormat = GUID_WICPixelFormat32bppPBGRA;
		hRes = frame->SetPixelFormat(&wicPixelFormat);
		if (hRes != S_OK)
			return FALSE;
		UINT stride = m_cx * 4;
		stride = ((stride + sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);
		hRes = frame->WritePixels(m_cy, stride, stride * m_cy, (BYTE*)m_bits);
		if (hRes != S_OK)
			return FALSE;
		hRes = frame->Commit();
		if (hRes != S_OK)
			return FALSE;
		hRes = encoder->Commit();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	HBITMAP TinyWICImage::GetHBITMAP()
	{
		return m_hBitmap;
	}
	BYTE*	TinyWICImage::GetBits()
	{
		return m_bits;
	}
	TinySize TinyWICImage::GetSize()
	{
		return TinySize(m_cx, m_cy);
	}
	TinyRectangle TinyWICImage::GetRectangle()
	{
		return TinyRectangle(0, 0, m_cx, m_cy);
	}
	void TinyWICImage::Close()
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		m_cx = m_cy = 0;
	}
}
