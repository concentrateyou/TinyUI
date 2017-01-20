#include "stdafx.h"
#include "GameScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(GameScene, DX11Image);
	GameScene::GameScene()
	{
	}


	GameScene::~GameScene()
	{
	}

	BOOL GameScene::Initialize(DX11& dx11)
	{
		m_captureTask.Reset(new DX11CaptureTask(&dx11, *this));
		return m_captureTask != NULL;
	}

	void GameScene::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_captureTask->SetConfig(className, exeName, dllName);
	}

	BOOL GameScene::Process(DX11& dx11)
	{
		return m_captureTask->Submit();
	}

	void GameScene::Clear(DX11& dx11)
	{
		m_captureTask->Close();
		m_captureTask.Reset(NULL);
	}

	BOOL GameScene::Render(DX11& dx11)
	{
		return DX11Image::Render(dx11);
	}
}
