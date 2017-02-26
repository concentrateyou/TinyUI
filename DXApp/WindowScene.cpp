#include "stdafx.h"
#include "WindowScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(WindowScene, DX11Image2D);

	WindowScene::WindowScene()
		:m_hWND(NULL)
	{
	}


	WindowScene::~WindowScene()
	{
	}

	BOOL WindowScene::Initialize(DX11& dx11, HWND hWND)
	{
		m_hWND = hWND;

		BOOL bEnable = FALSE;
		if (FAILED(DwmIsCompositionEnabled(&bEnable)))
			return FALSE;

		if (bEnable)
		{
			Destory();
			TinyScopedLibrary user32("user32.dll");
			DwmGetDxSharedSurface dwmGetDxSharedSurface = reinterpret_cast<DwmGetDxSharedSurface>(user32.GetFunctionPointer("DwmGetDxSharedSurface"));
			if (dwmGetDxSharedSurface != NULL)
			{
				HANDLE handle;
				LUID adapterLuid;
				ULONG fmtWindow;
				ULONG presentFlags;
				ULONGLONG win32kUpdateId;
				if (dwmGetDxSharedSurface(hWND, &handle, &adapterLuid, &fmtWindow, &presentFlags, &win32kUpdateId))
				{
					if (DX11Image2D::Load(dx11, handle))
					{
						DX11Image2D::Save(dx11, "D:\\12345.bmp");
						return TRUE;
					}
					return FALSE;
				}
			}
		}
		else
		{
			TinyRectangle rectangle;
			GetClientRect(m_hWND, &rectangle);
			if (!rectangle.IsRectEmpty())
			{
				Destory();
				return DX11Image2D::CreateCompatible(dx11, rectangle.Size());
			}
		}
		return FALSE;
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

	}

	BOOL WindowScene::Render(DX11& dx11)
	{
		BOOL bEnable = FALSE;
		if (FAILED(DwmIsCompositionEnabled(&bEnable)))
			return FALSE;
		if (bEnable)
		{
			DX11Image2D::Render(dx11);
			return TRUE;
		}
		else
		{
			HDC hDC = ::GetDC(m_hWND);
			if (hDC != NULL)
			{
				TinyRectangle rectangle;
				GetClientRect(m_hWND, &rectangle);
				TinyMemDC dc(hDC, TO_CX(rectangle), TO_CY(rectangle));
				PrintWindow(m_hWND, dc, 0);
				DX11Image2D::BitBlt(dx11, rectangle, hDC, TinyPoint(0, 0));
				::ReleaseDC(m_hWND, hDC);
				DX11Image2D::Render(dx11);
				return TRUE;
			}
		}
		return FALSE;
	}
}
