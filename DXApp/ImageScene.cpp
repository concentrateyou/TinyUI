#include "stdafx.h"
#include "ImageScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(ImageScene, DX11Image);
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

	BOOL ImageScene::Allocate(DX11& dx11)
	{
		return TRUE;
	}

	void ImageScene::Deallocate(DX11& dx11)
	{

	}

	BOOL ImageScene::Render(DX11& dx11)
	{
		return DX11Image::Render(dx11);
	}
}

