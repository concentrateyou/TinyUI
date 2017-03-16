#include "stdafx.h"
#include "WindowScene.h"
#include <d3d9types.h>

namespace DXApp
{
	IMPLEMENT_DYNAMIC(WindowScene, DX11Image2D);

	WindowScene::WindowScene()
		:m_hWND(NULL),
		m_hBitmap(NULL),
		m_bits(NULL)
	{
	}


	WindowScene::~WindowScene()
	{
	}

	BOOL WindowScene::Initialize(DX11& dx11, HWND hWND)
	{
		if (!IsWindow(hWND))
			return FALSE;
		Destory();
		m_hWND = hWND;
		GetClientRect(m_hWND, &m_snapshot);
		if (DX11Image2D::Create(dx11, m_snapshot.Size(), NULL, FALSE))
		{
			SAFE_DELETE_OBJECT(m_hBitmap);
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_size.cx;
			bmi.bmiHeader.biHeight = -m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 4;
			m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_bits), NULL, 0);
			m_memDC.Reset(new TinyMemDC(NULL, m_hBitmap));
			return TRUE;
		}
		return FALSE;
		//BOOL bEnable = FALSE;
		//if (FAILED(DwmIsCompositionEnabled(&bEnable)))
		//	return FALSE;
		//if (bEnable)
		//{
		//	//Ä³Ð©¶ÀÏÔÏÔ¿¨ºÚÆÁ
		//	TinyScopedLibrary user32("USER32.dll");
		//	DwmGetDxSharedSurface dwmGetDxSharedSurface = reinterpret_cast<DwmGetDxSharedSurface>(user32.GetFunctionPointer("DwmGetDxSharedSurface"));
		//	if (dwmGetDxSharedSurface != NULL)
		//	{
		//		HANDLE handle;
		//		LUID adapterLuid;
		//		ULONG fmtWindow;
		//		ULONG presentFlags;
		//		ULONGLONG win32kUpdateId;
		//		if (dwmGetDxSharedSurface(hWND, &handle, &adapterLuid, &fmtWindow, &presentFlags, &win32kUpdateId))
		//		{
		//			return DX11Image2D::Load(dx11, handle);
		//		}
		//	}
		//}
		//else
		//{
		//	GetClientRect(m_hWND, &m_snapshot);
		//	if (DX11Image2D::Create(dx11, m_snapshot.Size(), NULL, FALSE))
		//	{
		//		SAFE_DELETE_OBJECT(m_hBitmap);
		//		BITMAPINFO bmi = { 0 };
		//		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//		bmi.bmiHeader.biWidth = m_size.cx;
		//		bmi.bmiHeader.biHeight = -m_size.cy;
		//		bmi.bmiHeader.biPlanes = 1;
		//		bmi.bmiHeader.biBitCount = 32;
		//		bmi.bmiHeader.biCompression = BI_RGB;
		//		bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 4;
		//		m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_bits), NULL, 0);
		//		m_memDC.Reset(new TinyMemDC(NULL, m_hBitmap));
		//		return TRUE;
		//	}
		//}
		//return FALSE;
	}

	HWND WindowScene::GetCaptureWindow() const
	{
		return m_hWND;
	}

	BOOL WindowScene::Process(DX11& dx11)
	{
		return TRUE;
	}

	void WindowScene::Clear(DX11& dx11)
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		m_memDC.Reset(NULL);
	}

	BOOL WindowScene::Render(DX11& dx11)
	{
		HDC hDC = ::GetDC(m_hWND);
		if (hDC != NULL)
		{
			TinyRectangle rectangle;
			GetClientRect(m_hWND, &rectangle);
			if (rectangle != m_snapshot)
			{
				Initialize(dx11, m_hWND);
			}
			INT cx = TO_CX(m_snapshot);
			INT cy = TO_CY(m_snapshot);
			::BitBlt(m_memDC->Handle(), 0, 0, cx, cy, hDC, m_snapshot.Position().x, m_snapshot.Position().y, SRCCOPY | CAPTUREBLT);

			CURSORINFO ci;
			ZeroMemory(&ci, sizeof(ci));
			ci.cbSize = sizeof(ci);
			if (GetCursorInfo(&ci))
			{
				POINT pos = ci.ptScreenPos;
				ScreenToClient(m_hWND, &pos);
				if (ci.flags & CURSOR_SHOWING)
				{
					HICON hICON = CopyIcon(ci.hCursor);
					if (hICON != NULL)
					{
						DrawIcon(m_memDC->Handle(), pos.x, pos.y, hICON);
						DestroyIcon(hICON);
					}
				}
			}

			UINT  linesize = cx * 4;
			this->Copy(dx11, m_bits, linesize * m_size.cy, linesize);
			::ReleaseDC(m_hWND, hDC);
			DX11Image2D::Render(dx11);
			return TRUE;
		}
		return FALSE;
		/*BOOL bEnable = FALSE;
		if (FAILED(DwmIsCompositionEnabled(&bEnable)))
			return FALSE;
		if (bEnable)
		{

			DX11Image2D::Render(dx11);
			return TRUE;
		}
		else
		{

		}
		return FALSE;*/
	}
}
