#include "stdafx.h"
#include "DX11DWMImage.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11DWMImage, DX11Image);

	DX11DWMImage::DX11DWMImage()
	{
	}

	DX11DWMImage::~DX11DWMImage()
	{
	}

	BOOL DX11DWMImage::Initialize(DX11& dx11, HWND hWND)
	{
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
				return DX11Image::Load(dx11, handle);
			}
		}
		return FALSE;
	}
}

