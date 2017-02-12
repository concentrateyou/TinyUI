#include "stdafx.h"
#include "WindowScene.h"

namespace DXApp
{


	IMPLEMENT_DYNAMIC(WindowScene, DX11Image);

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

		TinyRectangle rectangle;
		GetClientRect(m_hWND, &rectangle);
		if (!rectangle.IsRectEmpty())
		{
			Destory();

			//TinyComPtr<IDXGIDevice> device;
			//dx11.GetD3D()->QueryInterface(__uuidof(IDXGIDevice), (LPVOID*)&device);

			//IDXGIAdapter* adapter;
			//device->GetAdapter(&adapter);
			//DXGI_ADAPTER_DESC desc;
			//adapter->GetDesc(&desc);

			//TinyScopedLibrary dwmapi("dwmapi.dll");
			//m_pDwmpDxGetWindowSharedSurface = (INT(WINAPI*)(HWND, LUID, DWORD, DWORD, DXGI_FORMAT*, HANDLE*, LPVOID*))dwmapi.GetFunctionPointer((LPCSTR)100);
			//m_pDwmpDxUpdateWindowSharedSurface = (INT(WINAPI*)(HWND, int, int, int, HMONITOR, void*))dwmapi.GetFunctionPointer((LPCSTR)101);

			//DXGI_FORMAT format;//=D3DFMT_UNKNOWN;
			//LUID luidN = { 0,0 };
			//m_pDwmpDxGetWindowSharedSurface(m_hWND, desc.AdapterLuid, 0, 0, &format, &m_handle, (LPVOID*)&luidN);
			//m_pDwmpDxUpdateWindowSharedSurface(m_hWND, 0, 0, 0, 0, 0);
			
			/*if (DX11Image::Load(dx11, m_handle))
			{
				m_texture.Save(dx11, "D:\\12345.bmp", D3DX11_IFF_BMP);
			}*/
			return DX11Image::CreateCompatible(dx11, rectangle.Size());
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
		HRESULT hRes = S_OK;
		HDC hDC = ::GetDC(m_hWND);
		if (hDC != NULL)
		{
			BOOL bEnable = FALSE;
			DwmIsCompositionEnabled(&bEnable);
			TinyRectangle rectangle;
			GetClientRect(m_hWND, &rectangle);
			if (bEnable)
			{
				DX11Image::BitBlt(dx11, rectangle, hDC, TinyPoint(0, 0));
			}
			else
			{
				TinyMemDC dc(hDC, TO_CX(rectangle), TO_CY(rectangle));
				PrintWindow(m_hWND, dc, PW_CLIENTONLY);
				DX11Image::BitBlt(dx11, rectangle, hDC, TinyPoint(0, 0));
			}
			::ReleaseDC(m_hWND, hDC);
			DX11Image::Render(dx11);
			return TRUE;
		}
		return FALSE;
	}
}
