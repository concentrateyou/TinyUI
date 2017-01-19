#include "stdafx.h"
#include "WindowScene.h"

namespace DXApp
{
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
			return DX11Image::Create(dx11, rectangle.Size(), NULL);
		}
		return FALSE;
	}

	LPCSTR WindowScene::GetClassName()
	{
		return TEXT("WindowScene");
	}

	BOOL WindowScene::BeginScene()
	{
		return TRUE;
	}

	void WindowScene::EndScene()
	{

	}

	BOOL WindowScene::Render(DX11& dx11)
	{
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			TinyRectangle rectangle;
			GetClientRect(m_hWND, &rectangle);
			DX11Image::BitBlt(dx11, rectangle, hDC, TinyPoint(0, 0));
			ReleaseDC(m_hWND, hDC);
			DX11Image::Render(dx11);
			return TRUE;
		}
		return FALSE;
	}
}
