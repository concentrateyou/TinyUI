#include "stdafx.h"
#include "ScreenScene.h"
#include "Utility.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(ScreenScene, DX11Image);
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
			m_snapshot = s;
			return DX11Image::CreateCompatible(dx11, m_snapshot.Size());
		}
		return FALSE;
	}

	BOOL ScreenScene::Process(DX11& dx11)
	{
		return TRUE;
	}
	void ScreenScene::Clear(DX11& dx11)
	{

	}
	BOOL ScreenScene::Render(DX11& dx11)
	{
		HDC hDC = GetDC(NULL);
		if (hDC != NULL)
		{
			TinyRectangle dst(TinyPoint(0, 0), m_snapshot.Size());
			DX11Image::BitBlt(dx11, dst, hDC, m_snapshot.Position());
			ReleaseDC(NULL, hDC);
			DX11Image::Render(dx11);
			return TRUE;
		}
		return FALSE;
	}
}
