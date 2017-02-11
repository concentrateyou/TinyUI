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
			return DX11Image::CreateCompatible(dx11, rectangle.Size());
		}
		return FALSE;
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
				DX11Image::BitBlt(dx11, rectangle, dc, TinyPoint(0, 0));
			}
			::ReleaseDC(m_hWND, hDC);
			DX11Image::Render(dx11);
			return TRUE;
		}
		return FALSE;
	}
}
