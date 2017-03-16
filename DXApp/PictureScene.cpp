#include "stdafx.h"
#include "PictureScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(PictureScene, DX11Image2D);

	PictureScene::PictureScene()
	{
	}


	PictureScene::~PictureScene()
	{
	}

	BOOL PictureScene::Initialize(DX11& dx11, const TinyString& imageFile)
	{
		Destory();
		return Load(dx11, imageFile.STR());
	}

	BOOL PictureScene::Process(DX11& dx11)
	{
		return TRUE;
	}

	void PictureScene::Clear(DX11& dx11)
	{

	}

	BOOL PictureScene::Render(DX11& dx11)
	{
		return DX11Image2D::Render(dx11);
	}
}

