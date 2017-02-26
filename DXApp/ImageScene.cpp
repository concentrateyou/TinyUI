#include "stdafx.h"
#include "ImageScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(ImageScene, DX11Image2D);
	ImageScene::ImageScene()
	{
	}


	ImageScene::~ImageScene()
	{
	}

	BOOL ImageScene::Initialize(DX11& dx11, const TinyString& imageFile)
	{
		Destory();
		return Load(dx11, imageFile.STR());
	}

	BOOL ImageScene::Process(DX11& dx11)
	{
		return TRUE;
	}

	void ImageScene::Clear(DX11& dx11)
	{

	}

	BOOL ImageScene::Render(DX11& dx11)
	{
		return DX11Image2D::Render(dx11);
	}
}

