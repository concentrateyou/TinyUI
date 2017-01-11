#include "stdafx.h"
#include "ImageScene.h"

namespace DXApp
{
	ImageScene::ImageScene()
	{
	}


	ImageScene::~ImageScene()
	{
	}

	BOOL ImageScene::Initialize(DX11& dx11, const TinyString& imageFile)
	{
		return Load(dx11, imageFile.STR());
	}

	BOOL ImageScene::BeginScene()
	{
		return TRUE;
	}

	void ImageScene::EndScene()
	{

	}

	BOOL ImageScene::Render(const DX11& dx11)
	{
		return DX11Image::Render(dx11);
	}

	LPCSTR ImageScene::GetClassName()
	{
		return TEXT("ImageScene");
	}
}

