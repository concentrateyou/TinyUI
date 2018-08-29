#include "stdafx.h"
#include "DX11Cursor2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Cursor2D, DX11Image2D);

	DX11Cursor2D::DX11Cursor2D()
	{
	}

	DX11Cursor2D::~DX11Cursor2D()
	{

	}

	BOOL DX11Cursor2D::GetCursor(HICON hICON, SIZE& size)
	{
		BITMAP bitmap;
		ICONINFO ii;
		ZeroMemory(&ii, sizeof(ii));
		if (!GetIconInfo(hICON, &ii))
			goto _ERROR;
		if (!::GetObject(ii.hbmMask, sizeof(bitmap), &bitmap))
			goto _ERROR;
		size.cx = bitmap.bmWidth;
		size.cy = bitmap.bmHeight / (ii.hbmColor ? 1 : 2);
		SAFE_DELETE_OBJECT(ii.hbmColor);
		SAFE_DELETE_OBJECT(ii.hbmMask);
		return TRUE;
	_ERROR:
		SAFE_DELETE_OBJECT(ii.hbmColor);
		SAFE_DELETE_OBJECT(ii.hbmMask);
		return FALSE;
	}
	BOOL DX11Cursor2D::AlphaPixels(const UINT32* pixels, UINT32 count)
	{
		for (const UINT32* ps = pixels + count; pixels != ps; ++pixels)
		{
			if ((*pixels & 0xff000000) != 0)
				return true;
		}
	}
	BOOL DX11Cursor2D::UpdateCursor(DX11& dx11, HICON hICON, const SIZE& size)
	{
		BITMAPV5HEADER h;
		ZeroMemory(&h, sizeof(h));
		h.bV5Size = sizeof(BITMAPV5HEADER);
		h.bV5Width = size.cx;
		h.bV5Height = -size.cy;
		h.bV5Planes = 1;
		h.bV5Compression = BI_RGB;
		h.bV5BitCount = 32;
		h.bV5RedMask = 0x00FF0000;
		h.bV5GreenMask = 0x0000FF00;
		h.bV5BlueMask = 0x000000FF;
		h.bV5AlphaMask = 0xFF000000;
		h.bV5CSType = LCS_WINDOWS_COLOR_SPACE;
		h.bV5Intent = LCS_GM_IMAGES;
		HDC hDC = ::GetDC(NULL);
		if (hDC != NULL)
		{
			UINT32 pixels = size.cx * size.cy;
			UINT32* bits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, reinterpret_cast<BITMAPINFO*>(&h), DIB_RGB_COLORS, reinterpret_cast<void**>(&bits), NULL, 0);
			ASSERT(hBitmap);
			memset(bits, 0, pixels * 4);
			TinyMemDC dc(hDC, hBitmap);
			::DrawIconEx(dc, 0, 0, hICON, size.cx, size.cy, 0, NULL, DI_MASK);
			TinyScopedArray<BOOL> opaque(new BOOL[pixels]);
			for (UINT32 i = 0; i < pixels; ++i)
				opaque[i] = !bits[i];
			memset(bits, 0, pixels * 4);
			::DrawIconEx(dc, 0, 0, hICON, size.cx, size.cy, 0, NULL, DI_NORMAL);
			if (!AlphaPixels(static_cast<const UINT32*>(bits), pixels))
			{
				UINT32* p = static_cast<UINT32*>(bits);
				for (UINT32 i = 0; i < pixels; ++p, ++i)
				{
					if (opaque[i])
						*p |= 0xff000000;
					else
						*p &= 0x00ffffff;
				}
			}
			D3D11_MAPPED_SUBRESOURCE map;
			if (Map(dx11, map, FALSE))
			{
				memcpy((BYTE*)map.pData, static_cast<void*>(bits), size.cx * size.cy * 4);
				Unmap(dx11);
			}
			SAFE_DELETE_OBJECT(hBitmap);
			::ReleaseDC(NULL, hDC);
		}
		return TRUE;
	}
	BOOL DX11Cursor2D::Create(DX11& dx11, HICON hICON)
	{
		SIZE size = { 0 };
		if (!GetCursor(hICON, size))
			return FALSE;
		if (!DX11Image2D::Create(dx11, size.cx, size.cy, NULL, FALSE))
			return FALSE;
		if (!UpdateCursor(dx11, hICON, size))
			return FALSE;
		return TRUE;

	}
}