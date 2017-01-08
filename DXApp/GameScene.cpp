#include "stdafx.h"
#include "GameScene.h"


GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

BOOL GameScene::Initialize(DX11& dx11)
{
	m_captureTask.Reset(new DX11CaptureTask(&dx11));
	return m_captureTask != NULL;
}

BOOL GameScene::BeginScene()
{
	return m_captureTask->Submit();
}

void GameScene::EndScene()
{
	m_captureTask->Close();
}

BOOL GameScene::Render(const DX11& dx11)
{
	DX11Image& image = m_captureTask->GetTexture();
	if (image.IsValid())
	{
		return image.Render(dx11);
	}
	return FALSE;
}

LPCSTR GameScene::GetClassName()
{
	return TEXT("GameScene");
}
