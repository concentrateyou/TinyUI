#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	typedef BOOL(WINAPI *DwmGetDxSharedSurface)(
		HWND hwnd,
		HANDLE* phSurface,
		LUID* pAdapterLuid,
		ULONG* pFmtWindow,
		ULONG* pPresentFlags,
		ULONGLONG* pWin32kUpdateId);



	class DX11DWMImage : public DX11Image
	{
		DECLARE_DYNAMIC(DX11DWMImage)
		DISALLOW_COPY_AND_ASSIGN(DX11DWMImage)
	public:
		DX11DWMImage();
		~DX11DWMImage();
		BOOL Initialize(DX11& dx11,HWND hWND);
	};
}



