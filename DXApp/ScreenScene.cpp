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
			return DX11Image::Create(dx11, s.Size(), NULL);
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
		HDC hDC = GetDC(NULL);
		if (hDC != NULL)
		{
			TinyRectangle s(TinyPoint(0, 0), m_screenRect.Size());
			this->BitBlt(dx11, hDC, m_screenRect, m_screenRect.Position());
			ReleaseDC(NULL, hDC);
			DX11Image::Render(dx11);
			return TRUE;
		}
		return FALSE;
	}
}
