#include "stdafx.h"
#include "ScreenScene.h"

namespace DXApp
{

	ScreenScene::ScreenScene()
	{
	}


	ScreenScene::~ScreenScene()
	{
	}

	BOOL ScreenScene::Initialize(DX11& dx11, const TinyRectangle& s)
	{
		Destory();
		if (!s.IsRectEmpty())
		{
			m_screenRect = s;
			return DX11Image::Create(dx11, m_screenRect.Size(), NULL);
		}
		return FALSE;
	}

	LPCSTR ScreenScene::GetClassName()
	{
		return TEXT("ScreenScene");
	}
	ElementType ScreenScene::GetElementType() const
	{
		return IMAGE;
	}
	BOOL ScreenScene::BeginScene()
	{
		return TRUE;
	}
	void ScreenScene::EndScene()
	{

	}
	BOOL ScreenScene::Render(const DX11& dx11)
	{
		HWND hWND = GetDesktopWindow();
		ASSERT(hWND);
		HDC hDC = GetDC(hWND);
		if (hDC != NULL)
		{
			this->BitBlt(dx11, hDC, m_screenRect);
			DX11Image::Render(dx11);
			ReleaseDC(hWND, hDC);
			hDC = NULL;
			return TRUE;
		}
		return FALSE;
	}
}
